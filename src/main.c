#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOTAS 7
#define MAX_COMMAND_LEN 20

#define CTRL_A 0x01
#define CTRL_X 0x18
#define ESC    0x1B

#define FILE_DNE -1

#define SAVE_FILE_NAME "save.txt"

void MostrarNotas(int moedaSelecionada);
int AbrirSaveFile(char* nomeFicheiro);
int SalvarSaveFile(char* nomeFicheiro);

int valorPorNota[MAX_NOTAS] = { 5, 10, 20, 50, 100, 200, 500 };
int totalQuantidadePorNota[MAX_NOTAS] = {0};
char moeda[3 + 1] = "EUR";

int main() {
        int key;
        int repeatTimes;
        int digit;
        int idxSelected = 0;
        char comando[MAX_COMMAND_LEN + 1];

        WINDOW* win = initscr();

        AbrirSaveFile(SAVE_FILE_NAME);

        noecho();
        keypad(win, true);
        curs_set(0);

        do {
                move(0, 0);
                MostrarNotas(idxSelected);

                repeatTimes = 0;
                do {
                        key = getch();
                        if ('0' <= key && key <= '9'
                            && repeatTimes < 99999) {
                                digit = key - 0x30;
                                repeatTimes = repeatTimes * 10 + digit;
                                addch(key);
                        }
                } while ('0' <= key && key <= '9');
                move(MAX_NOTAS + 3, 0);
                clrtoeol();

                if (repeatTimes == 0) {
                        repeatTimes = 1;
                }

                switch (key) {
                case KEY_DOWN:
                case 's':
                case 'j':
                        idxSelected += repeatTimes;
                        if (idxSelected >= MAX_NOTAS) {
                                idxSelected = MAX_NOTAS - 1;
                        }
                        break;
                case KEY_UP:
                case 'w':
                case 'k':
                        idxSelected -= repeatTimes;
                        if (idxSelected < 0) {
                                idxSelected = 0;
                        }
                        break;
                case '+':
                case CTRL_A:
                        totalQuantidadePorNota[idxSelected] += repeatTimes;
                        if (totalQuantidadePorNota[idxSelected] > 9999) {
                                totalQuantidadePorNota[idxSelected] = 9999;
                        }
                        break;
                case '-':
                case CTRL_X:
                        totalQuantidadePorNota[idxSelected] -= repeatTimes;
                        if (totalQuantidadePorNota[idxSelected] < -9999) {
                                totalQuantidadePorNota[idxSelected] = -9999;
                        }
                        break;
                case ':':
                        addch(':');
                        echo();
                        curs_set(1);
                        getstr(comando);
                        comando[MAX_COMMAND_LEN] = '\0';

                        if (strcmp(comando, "q") == 0
                            || strcmp(comando, "quit")) {
                                key = '\e';
                        }
                        break;
                }
        } while (key != '\e');
        addstr("Saving...\n");
        SalvarSaveFile(SAVE_FILE_NAME);
        endwin();

        return 0;
}

void MostrarNotas(int moedaSelecionada) {
        int totalQtd = 0;
        int totalVal = 0;
        int relativeNumber;
        for (int i = 0; i < sizeof(valorPorNota) / sizeof(valorPorNota[0]); i++) {
                relativeNumber = abs(i - moedaSelecionada);
                printw(i == moedaSelecionada ? "%-3d> " : "%3d| ",
                        i == moedaSelecionada ? i + 1 : relativeNumber
                );

                printw("%3d %3s   %6d   %8d.00 %3s %c\n",
                        valorPorNota[i],
                        moeda,
                        totalQuantidadePorNota[i],
                        totalQuantidadePorNota[i] * valorPorNota[i],
                        moeda,
                        i == moedaSelecionada ? '<' : '|'
                );
                totalQtd += totalQuantidadePorNota[i];
                totalVal += totalQuantidadePorNota[i] * valorPorNota[i];
        }
        printw("   +------------------------------------+\n");
        printw("   | SUM ALL   %6d   %8d.00 %3s |\n",
                totalQtd,
                totalVal,
                moeda
        );
        printw("   +------------------------------------+\n");
}
int AbrirSaveFile(char* nomeFicheiro) {
        char* caminho;
        caminho = malloc(strlen(getenv("HOME") + strlen("/.cache/") + strlen(nomeFicheiro) + 1));
        strcpy(caminho, getenv("HOME"));
        strcat(caminho, "/.cache/");
        strcat(caminho, nomeFicheiro);

        FILE* p_saveFile = fopen(caminho, "r");

        if (p_saveFile == NULL) {
                return -1;
        }

        if (fgets(moeda, 4, p_saveFile) == NULL) {
                return 1;
        }

        for (int i = 0; i < MAX_NOTAS; i++) {
                fscanf(p_saveFile, "%d %d",
                        &valorPorNota[i],
                        &totalQuantidadePorNota[i]
                );
        }
        fclose(p_saveFile);

        return 0;
}
int SalvarSaveFile(char* nomeFicheiro) {
        char* caminho;
        caminho = malloc(strlen(getenv("HOME") + strlen("/.cache/") + strlen(nomeFicheiro) + 1));
        strcpy(caminho, getenv("HOME"));
        strcat(caminho, "/.cache/");
        strcat(caminho, nomeFicheiro);

        FILE* p_saveFile = fopen(caminho, "w+");

        if (p_saveFile == NULL) {
                return -1;
        }
        fprintf(p_saveFile, "%3s\n",
                moeda
        );
        for (int i = 0; i < MAX_NOTAS; i++) {
                fprintf(p_saveFile, "%d %d\n",
                        valorPorNota[i],
                        totalQuantidadePorNota[i]
                );
        }
        fclose(p_saveFile);

        return 0;
}
