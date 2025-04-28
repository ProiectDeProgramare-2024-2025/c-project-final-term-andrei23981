#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>


#define COLOR_NAME "\033[38;5;51m"         // Albastru electric
#define COLOR_CAPACITY "\033[38;5;46m"     // Verde lime
#define COLOR_FACILITIES "\033[38;5;226m"  // Galben neon
#define COLOR_PRICE "\033[38;5;201m"       // Roz vibrant
#define COLOR_ERROR "\033[38;5;196m"       // Rosu aprins
#define COLOR_RESET "\033[0m"
#define COLOR_TITLE "\033[1;36m"           // Cyan bold
#define COLOR_PROMPT "\033[38;5;208m"      // Orange
#define COLOR_OPTION_NUM "\033[1;33m"      // Galben bold
#define COLOR_OPTION_TEXT "\033[38;5;39m"  // Albastru deschis

struct Hall {
    char name[100];
    int capacity;
    char facilities[200];
    float price_per_hour;
};

struct Reservation {
    char hall_name[100];
    char date[11];
    char start_time[6];
    char end_time[6];
};

void clearScreen() {
    system("cls||clear");
}

int readHalls(struct Hall *halls, int *count) {
    FILE *file = fopen("halls.txt", "r");
    if (!file) return -1;
    *count = 0;
    char line[256];
    while (fgets(line, sizeof(line), file) && *count < 100) {
        char *name = strtok(line, ";");
        char *cap = strtok(NULL, ";");
        char *facilities = strtok(NULL, ";");
        char *price = strtok(NULL, "\n");
        if (name && cap && facilities && price) {
            strncpy(halls[*count].name, name, sizeof(halls[*count].name));
            halls[*count].capacity = atoi(cap);
            strncpy(halls[*count].facilities, facilities, sizeof(halls[*count].facilities));
            halls[*count].price_per_hour = atof(price);
            (*count)++;
        }
    }
    fclose(file);
    return 0;
}
void deleteReservation() {
    clearScreen();
    printf(COLOR_TITLE "\n========== STERGERE REZERVARE ==========\n" COLOR_RESET);
    FILE *f = fopen("rezervari.txt", "r");
    if (!f) {
        printf(COLOR_ERROR "Nu exista rezervari!\n" COLOR_RESET);
        return;
    }

    struct Reservation reservations[100];
    int count = 0;
    char line[256];
    while (fgets(line, sizeof(line), f) && count < 100) {
        sscanf(line, "%[^;];%[^;];%[^;];%[^\n]", reservations[count].hall_name,
            reservations[count].date, reservations[count].start_time,
            reservations[count].end_time);
        count++;
    }
    fclose(f);

    if (count == 0) {
        printf(COLOR_ERROR "Nu exista rezervari!\n" COLOR_RESET);
        return;
    }

    for (int i = 0; i < count; i++) {
        printf(COLOR_OPTION_NUM "\n%d. " COLOR_RESET, i+1);
        printf(COLOR_NAME "%s" COLOR_RESET " - " COLOR_CAPACITY "%s %s-%s\n",
            reservations[i].hall_name, reservations[i].date,
            reservations[i].start_time, reservations[i].end_time);
    }

    int choice = getIntInput("\nIntrodu numarul rezervarii de sters: ", 1);
    if (choice < 1 || choice > count) {
        printf(COLOR_ERROR "Numar invalid!\n" COLOR_RESET);
        return;
    }

    f = fopen("rezervari.txt", "w");
    for (int i = 0; i < count; i++) {
        if (i != choice-1) {
            fprintf(f, "%s;%s;%s;%s\n", reservations[i].hall_name,
                reservations[i].date, reservations[i].start_time,
                reservations[i].end_time);
        }
    }
    fclose(f);
    printf(COLOR_CAPACITY "\n>>> Rezervare stearsa cu succes! <<<\n" COLOR_RESET);
}
void displayHalls() {
    clearScreen();
    struct Hall halls[100];
    int count;
    printf(COLOR_TITLE "\n========== SALI DISPONIBILE ==========\n" COLOR_RESET);
    if (readHalls(halls, &count) == -1) {
        printf(COLOR_ERROR "Eroare: Nu s-a putut deschide fisierul halls.txt!\n" COLOR_RESET);
        printf("Exemplu fisier:\nSala Alfa;50;Proiector,Tabla;100\n");
        return;
    }
    if (count == 0) {
        printf(COLOR_ERROR "Nu exista sali disponibile!\n" COLOR_RESET);
        return;
    }
    for (int i = 0; i < count; i++) {
        printf("\n>>> " COLOR_NAME "%s" COLOR_RESET
               "\n    " COLOR_CAPACITY "Capacitate: %d" COLOR_RESET
               "\n    " COLOR_FACILITIES "Facilitati: %s" COLOR_RESET
               "\n    " COLOR_PRICE "Pret/ora: %.2f RON" COLOR_RESET "\n",
               halls[i].name, halls[i].capacity,
               halls[i].facilities, halls[i].price_per_hour);
    }
}

int getIntInput(const char *prompt, int min) {
    int value;
    while (1) {
        printf(COLOR_PROMPT "%s" COLOR_RESET, prompt);
        if (scanf("%d", &value) != 1 || value < min) {
            printf(COLOR_ERROR "Input invalid. Introdu un numar >= %d.\n" COLOR_RESET, min);
            while (getchar() != '\n');
        } else {
            while (getchar() != '\n');
            return value;
        }
    }
}

void searchByCapacity(struct Hall *halls, int count) {
    int target = getIntInput("Introdu capacitatea minima: ", 1);
    printf(COLOR_TITLE "\n========== SALI GASITE (Capacitate >= %d) ==========\n" COLOR_RESET, target);
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (halls[i].capacity >= target) {
            printf(">>> " COLOR_NAME "%s" COLOR_RESET " (" COLOR_CAPACITY "%d locuri" COLOR_RESET ")\n",
                   halls[i].name, halls[i].capacity);
            found = 1;
        }
    }
    if (!found) printf(COLOR_ERROR "Nicio sala gasita.\n" COLOR_RESET);
}

void searchByFacilities(struct Hall *halls, int count) {
    char facilities[100];
    printf(COLOR_PROMPT "Introdu facilitati cautate (ex: Proiector): " COLOR_RESET);
    fgets(facilities, sizeof(facilities), stdin);
    facilities[strcspn(facilities, "\n")] = 0;
    printf(COLOR_TITLE "\n========== SALI GASITE PENTRU: %s ==========\n" COLOR_RESET, facilities);
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strstr(halls[i].facilities, facilities)) {
            printf(">>> " COLOR_NAME "%s" COLOR_RESET "\n    " COLOR_FACILITIES "%s" COLOR_RESET "\n",
                   halls[i].name, halls[i].facilities);
            found = 1;
        }
    }
    if (!found) printf(COLOR_ERROR "Nicio sala gasita.\n" COLOR_RESET);
}

void searchHalls() {
    clearScreen();
    struct Hall halls[100];
    int count;
    if (readHalls(halls, &count) == -1) {
        printf(COLOR_ERROR "Eroare la citirea salilor!\n" COLOR_RESET);
        return;
    }
    printf(COLOR_TITLE "=== CAUTARE SALI ===\n" COLOR_RESET);
    printf("1. Cautare dupa capacitate\n2. Cautare dupa facilitati\n");
    int choice = getIntInput("Alegere (1-2): ", 1);
    if (choice == 1) searchByCapacity(halls, count);
    else if (choice == 2) searchByFacilities(halls, count);
    else printf(COLOR_ERROR "Optiune invalida!\n" COLOR_RESET);

}

int validateTime(const char *time) {
    if (strlen(time) != 5 || time[2] != ':') return 0;
    int hours = atoi(time);
    int mins = atoi(time + 3);
    return (hours >= 0 && hours < 24 && mins >= 0 && mins < 60);
}

void addReservation(const char *hall_name) {
    struct Reservation res;
    strcpy(res.hall_name, hall_name);
    while (1) {
        printf(COLOR_PROMPT "Introdu data rezervarii (zz/ll/aaaa): " COLOR_RESET);
        scanf("%10s", res.date);
        if (strlen(res.date) == 10 && res.date[2] == '/' && res.date[5] == '/') break;
        printf(COLOR_ERROR "Format invalid! Foloseste zz/ll/aaaa\n" COLOR_RESET);
    }
    while (1) {
        printf(COLOR_PROMPT "Ora inceput (HH:MM): " COLOR_RESET);
        scanf("%5s", res.start_time);
        printf(COLOR_PROMPT "Ora sfarsit (HH:MM): " COLOR_RESET);
        scanf("%5s", res.end_time);
        if (validateTime(res.start_time) && validateTime(res.end_time)) break;
        printf(COLOR_ERROR "Format ore invalid!\n" COLOR_RESET);
    }
    FILE *f = fopen("rezervari.txt", "a");
    if (f) {
        fprintf(f, "%s;%s;%s;%s\n", res.hall_name, res.date, res.start_time, res.end_time);
        fclose(f);
        printf(COLOR_CAPACITY "\n>>> Rezervare salvata cu succes! <<<\n" COLOR_RESET);
    }
}

void viewReservations() {
    clearScreen();
    printf(COLOR_TITLE "========== LISTA REZERVARI ==========\n" COLOR_RESET);
    FILE *f = fopen("rezervari.txt", "r");
    if (!f) {
        printf(COLOR_ERROR "Nu exista rezervari!\n" COLOR_RESET);
        return;
    }
    char line[256];
    int found = 0;

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;

        char *name = strtok(line, ";");
        char *date = strtok(NULL, ";");
        char *start = strtok(NULL, ";");
        char *end = strtok(NULL, "\n");

        if (name && date && start && end) {
            printf("\n>>> " COLOR_NAME "%s" COLOR_RESET
                   "\n    " COLOR_CAPACITY "Data: %s" COLOR_RESET
                   "\n    " COLOR_FACILITIES "Interval: %s - %s" COLOR_RESET "\n",
                   name, date, start, end);
            found = 1;
        }
    }
    fclose(f);

    if (!found) {
        printf(COLOR_ERROR "\nNu s-au gasit rezervari valide in fisier!\n" COLOR_RESET);
    }
}

void makeReservation() {
    clearScreen();
    struct Hall halls[100];
    int count;
    if (readHalls(halls, &count)) {
        printf(COLOR_ERROR "Nu exista sali disponibile!\n" COLOR_RESET);
        return;
    }
    char another = 'y';
    do {
        clearScreen();
        displayHalls();
        char target[100];
        printf(COLOR_PROMPT "\nIntrodu numele salii dorite: " COLOR_RESET);
        int c;
        fgets(target, sizeof(target), stdin);
        target[strcspn(target, "\n")] = 0;

        int found = 0;
        for (int i = 0; i < count; i++) {
            if (strcmp(halls[i].name, target) == 0) {
                addReservation(halls[i].name);
                found = 1;
                break;
            }
        }
        if (!found) printf(COLOR_ERROR "\nSala nu a fost gasita!" COLOR_RESET);

        printf(COLOR_PROMPT "\nVrei sa adaugi alta rezervare? (y/n): " COLOR_RESET);
        scanf(" %c", &another);
        while ((c = getchar()) != '\n' && c != EOF);

    } while (tolower(another) == 'y');

}

int main() {
    int choice;
    do {
        clearScreen();
        printf(COLOR_TITLE "\n=========== MENIU PRINCIPAL ===========\n" COLOR_RESET);
        printf(COLOR_OPTION_NUM "1. " COLOR_OPTION_TEXT "Afisare sali\n");
        printf(COLOR_OPTION_NUM "2. " COLOR_OPTION_TEXT "Cautare sali\n");
        printf(COLOR_OPTION_NUM "3. " COLOR_OPTION_TEXT "Rezervare sala\n");
        printf(COLOR_OPTION_NUM "4. " COLOR_OPTION_TEXT "Vizualizare rezervari\n");
        printf(COLOR_OPTION_NUM "5. " COLOR_OPTION_TEXT "Stergere rezervare\n");
        printf(COLOR_OPTION_NUM "6. " COLOR_OPTION_TEXT "Iesire\n" COLOR_RESET);

        choice = getIntInput(COLOR_PROMPT "\nAlegere (1-6): " COLOR_RESET, 1);
        switch (choice) {
            case 1: displayHalls(); break;
            case 2: searchHalls(); break;
            case 3: makeReservation(); break;
            case 4: viewReservations(); break;
            case 5: deleteReservation(); break;
            case 6: printf(COLOR_TITLE "\nLa revedere!\n" COLOR_RESET); break;
            default: printf(COLOR_ERROR "Optiune invalida!\n" COLOR_RESET);
        }

        if (choice != 6) {
            printf(COLOR_PROMPT "\nApasa Enter pentru a reveni..." COLOR_RESET);
            getchar();
        }
    } while (choice != 6);
    return 0;
}
