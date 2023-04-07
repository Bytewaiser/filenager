#include <curses.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

    int ch;
    ESCDELAY = 0;
    char *lines[256];
    char *path = malloc(1);
    path = ".";
    if (!initscr())
        fprintf(stderr, "Error initializing curses screen\n");
    keypad(stdscr, TRUE);
    noecho();

    DIR *folder;
    if ((folder = opendir(path)) == NULL) {
        fprintf(stderr, "Error reading the folder\n");
        return 1;
    }

    int row = 0;
    int cursor_row = 0;

    struct dirent *entry;
    while ((entry = readdir(folder))) {
        lines[row] = malloc(sizeof(strlen(entry->d_name)));
        lines[row] = entry->d_name;
        mvprintw(row++, 0, "%s", entry->d_name);
    }
    closedir(folder);

    mvchgat(0, 0, -1, A_STANDOUT, 1, NULL);
    move(0, 0);
    while ((ch = getch()) != 27) {
        mvchgat(cursor_row, 0, -1, A_NORMAL, 1, NULL);
        if (ch == KEY_UP && cursor_row > 0) {
            cursor_row -= 1;
        }
        if (ch == KEY_DOWN && cursor_row < row - 1) {
            cursor_row += 1;
        }

        if (ch == 10) {
            clear();
            char *old_path = malloc(sizeof(path));
            strcpy(old_path, path);
            path = malloc(strlen(lines[cursor_row] + strlen(path) + 2));
            strcat(path, old_path);
            strcat(path, "/");
            strcat(path, lines[cursor_row]);
            printf("%s", path);
            if ((folder = opendir(path))== NULL) {
                fprintf(stderr, "Error reading the folder\n");
                return 1;
            }
            row = 0;

            while ((entry = readdir(folder))) {
                lines[row] = malloc(sizeof(strlen(entry->d_name)));
                lines[row] = entry->d_name;
                mvprintw(row++, 0, "%s", entry->d_name);
            }
            closedir(folder);
        }

        mvchgat(cursor_row, 0, -1, A_STANDOUT, 1, NULL);
        move(cursor_row, 0);
        refresh();
    }

    endwin();

    return 0;
}
