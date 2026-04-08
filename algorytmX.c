/* zadanie_interactive.c
   Prosty backtracking (Algorytm X - podstawowy) z wczytywaniem przez getchar().
   Kończy czytanie danych gdy:
     - napotka EOF (typowe w testach), lub
     - napotka pusty wiersz (użyteczne przy interakcyjnym wklejaniu; naciśnij Enter na pustej linii).
   Kompilacja: gcc @opcje zadanie_interactive.c -o zadanie_interactive
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ROWS 200
#define MAX_D   300

static char filter[MAX_D + 1];
static char rows[MAX_ROWS][MAX_D + 1];
static int row_count = 0;
static int d = 0;

/* struktury dla backtrackingu */
static int cover[MAX_D];
static char res_char[MAX_D];
static int used_row[MAX_ROWS];

/* czyta linię przez getchar(), zapisuje bez '\n'
   zwraca:
     -1 : EOF i nic nie odczytano
      0 : odczytano pusty wiersz (tylko '\n')
     >0 : długość wczytanej linii (bez '\n')
*/
static int read_line(char *buf, int maxlen) {
    int c;
    int pos = 0;
    int saw_any = 0;
    while ((c = getchar()) != EOF) {
        saw_any = 1;
        if (c == '\r') continue;
        if (c == '\n') break;
        if (pos < maxlen) buf[pos++] = (char)c;
        /* jeśli linia dłuższa niż maxlen, dalej ją obcinamy (ale czytamy do końca) */
    }
    if (c == EOF && !saw_any) return -1; /* EOF bez danych */
    buf[pos] = '\0';
    return pos;
}

/* wybiera pierwszą niepokrytą kolumnę (od lewej), lub -1 gdy wszystkie pokryte */
static int pick_column(void) {
    for (int i = 0; i < d; ++i) if (!cover[i]) return i;
    return -1;
}

/* przy znalezieniu pełnego pokrycia buduje wynik po filtrze i wypisuje go */
static void print_solution(void) {
    char out[MAX_D + 1];
    int p = 0;
    for (int i = 0; i < d; ++i) {
        if (filter[i] == '+') out[p++] = res_char[i];
    }
    out[p] = '\0';
    puts(out);
}

/* prosty DFS: wybieramy pierwszą niepokrytą kolumnę, próbujemy wiersze w porządku wejściowym */
static void dfs(int covered_count) {
    if (covered_count == d) {
        print_solution();
        return;
    }
    int c = pick_column();
    if (c == -1) return;

    for (int r = 0; r < row_count; ++r) {
        if (used_row[r]) continue;
        if (rows[r][c] == '_') continue;

        /* czy wiersz r koliduje z już pokrytymi kolumnami? */
        int conflict = 0;
        for (int j = 0; j < d; ++j) {
            if (rows[r][j] != '_' && cover[j]) { conflict = 1; break; }
        }
        if (conflict) continue;

        /* wybieramy wiersz r */
        used_row[r] = 1;
        int newly = 0;
        for (int j = 0; j < d; ++j) {
            if (rows[r][j] != '_') {
                cover[j] = 1;
                res_char[j] = rows[r][j];
                ++newly;
            }
        }

        dfs(covered_count + newly);

        /* cofamy wybór */
        for (int j = 0; j < d; ++j) {
            if (rows[r][j] != '_') cover[j] = 0;
        }
        used_row[r] = 0;
    }
}

int main(void) {
    /* wczytaj filtr */
    int len = read_line(filter, MAX_D);
    if (len == -1) return 0; /* brak danych (EOF) */
    if (len == 0) return 0;  /* pusty filtr traktujemy jako brak danych */
    d = len;
    if (d > MAX_D) return 1;

    /* wczytaj wiersze danych aż EOF lub pusty wiersz */
    char tmp[MAX_D + 1];
    while (1) {
        int l = read_line(tmp, MAX_D);
        if (l == -1) break; /* EOF */
        if (l == 0) break;  /* pusty wiersz -> traktujemy jako koniec wprowadzania (użytkownik) */
        if (l != d) {
            /* dane zadania zakładają jednakowe długości; jeśli różne -> błąd */
            return 1;
        }
        if (row_count >= MAX_ROWS) break;
        strcpy(rows[row_count], tmp);
        ++row_count;
    }

    if (row_count == 0) return 0;

    /* sprawdzenie, czy każda kolumna ma przynajmniej jednego kandydata */
    for (int j = 0; j < d; ++j) {
        int ok = 0;
        for (int r = 0; r < row_count; ++r) if (rows[r][j] != '_') { ok = 1; break; }
        if (!ok) return 0; /* brak rozwiązania */
    }

    /* inicjalizacja i uruchomienie DFS */
    for (int i = 0; i < d; ++i) { cover[i] = 0; res_char[i] = '?'; }
    for (int r = 0; r < row_count; ++r) used_row[r] = 0;

    dfs(0);

    return 0;
}
