#include <curses.h>
#include <dirent.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct Line {
    int row;
    int size;
    char name[128];
} Line;

typedef struct Lines {
    int row_count;
    char **files;
} Lines;

void init_curses() {
    ESCDELAY = 0;
    if (!initscr())
        fprintf(stderr, "Error initializing the application\n");
    keypad(stdscr, TRUE);
    noecho();
}

int compare_strings(const void *a, const void *b) {
    char const *pp1 = a;
    char const *pp2 = b;
    return strcmp(pp1, pp2);
}

void write_lines(Lines lines) {
    for (int i = 1; i < lines.row_count+1; i++) {
        mvprintw(i, 0, "%s", lines.files[i-1]);
    }
    mvchgat(1, 0, -1, A_STANDOUT, 1, NULL);
    move(1, 0);
}

Lines get_files(char *path) {
    DIR *folder;
    if ((folder = opendir(path)) == NULL) {
        fprintf(stderr, "Error reading the folder: %s\n", path);
        exit(1);
    }

    size_t l = 0;
    size_t max_len = 0;
    size_t row_count = 0;
    struct dirent *entry;
    while ((entry = readdir(folder))) {
        row_count++;
        l = strlen(entry->d_name);
        max_len = max_len > l ? max_len : l;
    }
    rewinddir(folder);

    char file_paths[row_count][max_len+1];

    size_t row = 0;
    while ((entry = readdir(folder))) {
        strcpy(file_paths[row++], entry->d_name);
    }

    qsort(file_paths, row_count, max_len+1, compare_strings);

    Lines lines;
    lines.row_count = row_count;
    lines.files = malloc(sizeof(char *) * row_count);
    for (size_t i = 0; i < row_count; i++) {
        lines.files[i] = malloc(max_len+1);
        strcpy(lines.files[i], file_paths[i]);
    }

    closedir(folder);
    return lines;
}

void cut_path(char *path) {
    size_t l = strlen(path);
    size_t cut_index = 0;
    for (size_t i = l; i > 0; i--) {
        if (path[i] == '/') {
            cut_index = i;
            break;
        }
    }
    if (cut_index == 0) {
        path[0] = '/';
        path[1] = '\0';
    } else {
        path[cut_index] = '\0';
    }
}

int main() {

    init_curses();
    Lines lines = get_files(".");
    write_lines(lines);
    char current_path[PATH_MAX];
    getcwd(current_path, PATH_MAX);
    mvprintw(0, 0, "%s\n", current_path);

    int ch;
    int cursor_row = 0;

    while ((ch = getch()) != 27) {
        mvchgat(cursor_row+1, 0, -1, A_NORMAL, 1, NULL);
        if (ch == KEY_UP && cursor_row > 0) {
            cursor_row -= 1;
        }
        if (ch == KEY_DOWN && cursor_row < lines.row_count - 1) {
            cursor_row += 1;
        }

        if (ch == 10) {
            if (!strcmp(lines.files[cursor_row], "."))
                ;
            else if (!strcmp(lines.files[cursor_row], "..")) {
                clear();
                cut_path(current_path);
                lines = get_files(current_path);
                write_lines(lines);
            } else {
                clear();
                if (strcmp(current_path, "/"))
                    strcat(current_path, "/");
                strcat(current_path, lines.files[cursor_row]);
                lines = get_files(current_path);
                write_lines(lines);
            }
            cursor_row = 0;
        }

        mvprintw(0, 0, "%s\n", current_path);
        mvchgat(cursor_row+1, 0, -1, A_STANDOUT, 1, NULL);
        move(cursor_row+1, 0);
        refresh();
    }

    endwin();

    return 0;
}
