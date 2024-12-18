#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_LENGTH 100

// Makan newline
void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

// Fungsi untuk nge cek username yang sama di file
int username_exists(const char *username) {
    FILE *file = fopen("credentials.txt", "r");
    if (!file) {
        perror("Error opening credentials file");
        return -1; // Return -1 untuk error
    }

    char line[MAX_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        trim_newline(line);
        char *stored_username = strtok(line, ":");
        if (stored_username && strcmp(username, stored_username) == 0) {
            fclose(file);
            return 1; // Username ada
        }
    }

    fclose(file);
    return 0; // Username gak ada
}

// Function sign-up
int sign_up(const char *username, const char *password) {
    if (username_exists(username) == 1) {
        printf("Username '%s' already exists. Please try a different one.\n", username);
        return 0;
    }

    FILE *file = fopen("credentials.txt", "a");
    if (!file) {
        perror("Error opening credentials file for writing");
        return -1;
    }

    fprintf(file, "%s:%s\n", username, password);
    fclose(file);

    printf("Account created successfully!\n");
    return 1;
}

// Function validasi sign-in
int sign_in(const char *username, const char *password) {
    FILE *file = fopen("credentials.txt", "r");
    if (!file) {
        perror("Error opening credentials file");
        return -1;
    }

    char line[MAX_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        trim_newline(line);
        char *stored_username = strtok(line, ":");
        char *stored_password = strtok(NULL, ":");

        if (stored_username && stored_password) {
            if (strcmp(username, stored_username) == 0 && strcmp(password, stored_password) == 0) {
                fclose(file);
                return 1; // akunnya valid dan ada
            }
        }
    }

    fclose(file);
    return 0; // Invalid
}

int main(void) {
    char choice;
    int result;
    char username[MAX_LENGTH], password[MAX_LENGTH];

    printf(" ===== ================== =====\n");
    printf(" ===== WELCOME TO LIBRARY =====\n");
    printf(" ===== ================== =====\n\n");
    getch();
    system("cls");

    printf(" ===== =================== ====\n");
    printf(" =====   LOGIN REQUIRED    ====\n");
    printf(" ===== =================== ====\n");

    while (1) {
        printf("---- ---------------- ----\n");
        printf("---- [1]: Sign in     ----\n");
        printf("---- [2]: Sign up     ----\n");
        printf("---- [Q]: Exit        ----\n");
        printf("----   Please pick    ----\n");

        scanf(" %c", &choice);
        getchar();

        choice = toupper(choice);

        switch (choice) {
            case '1':
                printf("Sign in selected.\n");
                printf("Enter username: ");
                fgets(username, MAX_LENGTH, stdin);
                trim_newline(username);

                printf("Enter password: ");
                fgets(password, MAX_LENGTH, stdin);
                trim_newline(password);

                result = sign_in(username, password);
                if (result == 1) {
                    printf("Login successful! Welcome, %s.\n", username);
                } else if (result == 0) {
                    printf("Invalid username or password.\n");
                } else {
                    printf("An error occurred while processing your login.\n");
                }
                break;

            case '2':
                printf("Sign up selected.\n");
                printf("Enter new username: ");
                fgets(username, MAX_LENGTH, stdin);
                trim_newline(username);

                printf("Enter new password: ");
                fgets(password, MAX_LENGTH, stdin);
                trim_newline(password);

                result = sign_up(username, password);
                if (result == 1) {
                    printf("You can now sign in with your new credentials.\n");
                } else {
                    printf("Sign-up failed. Please try again.\n");
                }
                break;

            case 'Q':
                printf("Exiting...\n");
                return 0;

            default:
                printf("Invalid option. Please try again.\n");
                break;
        }
    }

    return 0;
}
