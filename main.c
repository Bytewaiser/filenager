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
    Line lines[128];
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

void write_lines(Line *lines, int count) {
    for (int i = 0; i < count; i++) {
        mvprintw(lines[i].row, 0, "%s", lines[i].name);
    }
    mvchgat(0, 0, -1, A_STANDOUT, 1, NULL);
    move(0, 0);
}

void get_files(char *path) {
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

    char file_paths[row_count][max_len];

    size_t row = 0;
    while ((entry = readdir(folder))) {
        strcpy(file_paths[row++], entry->d_name);
    }

    qsort(file_paths, row_count, max_len, compare_strings);

    for (size_t i = 0; i < row_count; i++) {
        printf("%s\n", file_paths[i]);
    }

    closedir(folder);
}

void cut_path(char *path, char *new_path) {
    size_t l = strlen(path);
    size_t cut_index;
    for (size_t i = l; i > 0; i--) {
        if (path[i] == '/') {
            cut_index = i;
            break;
        }
    }
    strncpy(new_path, path, cut_index);
}

int main() {

    get_files(".");
    char old_path[128];
    char new_path[128];
    getcwd(old_path, 128);

    // cut_path(full_path, new_path);
    // printf("%s\n", new_path);

    // int ch;
    // int cursor_row = 0;

    // init_curses();
    // Lines lines = get_lines(".");
    // write_lines(lines, row_count);

    // for (int i=0; i<lines.line_count; i++) {
    //     printf("%s\n", lines.lines[i].name);
    // }

    // while ((ch = getch()) != 27) {
    //     mvchgat(cursor_row, 0, -1, A_NORMAL, 1, NULL);
    //     if (ch == KEY_UP && cursor_row > 0) {
    //         cursor_row -= 1;
    //     }
    //     if (ch == KEY_DOWN && cursor_row < row_count - 1) {
    //         cursor_row += 1;
    //     }
    //
    //     mvchgat(cursor_row, 0, -1, A_STANDOUT, 1, NULL);
    //     move(cursor_row, 0);
    //     refresh();
    // }
    //
    // endwin();

    return 0;
}
