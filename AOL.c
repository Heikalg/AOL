#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_LENGTH 100

// Fungsi untuk menghapus newline
void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

// Fungsi untuk mengecek apakah username sudah ada di file
int username_exists(const char *username) {
    FILE *file = fopen("credentials.txt", "r");
    if (!file) {
        perror("Error opening credentials file");
        return -1;
    }

    char line[MAX_LENGTH];
    char stored_username[MAX_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        trim_newline(line);
        if (sscanf(line, "%[^:]:", stored_username) == 1) {
            if (strcmp(username, stored_username) == 0) {
                fclose(file);
                return 1;
            }
        }
    }

    fclose(file);
    return 0;
}

// Fungsi untuk sign-up
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

// Fungsi untuk validasi sign-in
int sign_in(const char *username, const char *password) {
    FILE *file = fopen("credentials.txt", "r");
    if (!file) {
        perror("Error opening credentials file");
        return -1;
    }

    char line[MAX_LENGTH];
    char stored_username[MAX_LENGTH], stored_password[MAX_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        trim_newline(line);
        if (sscanf(line, "%[^:]:%s", stored_username, stored_password) == 2) {
            if (strcmp(username, stored_username) == 0 && strcmp(password, stored_password) == 0) {
                fclose(file);
                return 1;
            }
        }
    }

    fclose(file);
    return 0;
}

// Fungsi untuk menampilkan daftar buku
void list_books() {
    FILE *file = fopen("books.txt", "r");
    if (!file) {
        perror("Error opening books file");
        return;
    }

    char line[MAX_LENGTH];
    printf("\nAvailable Books:\n");
    while (fgets(line, sizeof(line), file)) {
        printf("- %s", line);
    }

    fclose(file);
    printf("\n");
}

// Fungsi untuk meminjam buku
void borrow_book(const char *username) {
    FILE *file = fopen("books.txt", "r");
    if (!file) {
        perror("Error opening books file");
        return;
    }

    char line[MAX_LENGTH];
    char books[MAX_LENGTH][MAX_LENGTH];
    int book_count = 0;

    printf("\nAvailable Books:\n");
    while (fgets(line, sizeof(line), file)) {
        trim_newline(line);
        strcpy(books[book_count], line);
        printf("[%d] %s\n", book_count + 1, line);
        book_count++;
    }
    fclose(file);

    if (book_count == 0) {
        printf("No books are currently available for borrowing.\n");
        return;
    }

    int choice;
    printf("\nEnter the number of the book you want to borrow: ");
    scanf("%d", &choice);
    getchar(); // Consume newline character left in the buffer

    if (choice < 1 || choice > book_count) {
        printf("Invalid choice. Please try again.\n");
        return;
    }

    char *book_title = books[choice - 1];

    file = fopen("books.txt", "r");
    if (!file) {
        perror("Error reopening books file");
        return;
    }

    FILE *temp = fopen("temp_books.txt", "w");
    if (!temp) {
        perror("Error creating temporary file");
        fclose(file);
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        trim_newline(line);
        if (strcmp(line, book_title) != 0) {
            fprintf(temp, "%s\n", line);
        }
    }

    fclose(file);
    fclose(temp);

    remove("books.txt");
    rename("temp_books.txt", "books.txt");

    FILE *loans = fopen("loans.txt", "a");
    if (!loans) {
        perror("Error opening loans file");
        return;
    }

    fprintf(loans, "%s:%s\n", username, book_title);
    fclose(loans);

    printf("You have successfully borrowed '%s'.\n", book_title);
    system("cls");
}


// Fungsi untuk mengembalikan buku
void return_book(const char *username) {
    FILE *loans = fopen("loans.txt", "r");
    if (!loans) {
        perror("Error opening loans file");
        return;
    }

    char line[MAX_LENGTH];
    char loaned_books[MAX_LENGTH][MAX_LENGTH];
    int loan_count = 0;

    printf("\nYour Borrowed Books:\n");
    while (fgets(line, sizeof(line), loans)) {
        trim_newline(line);

        char loaned_username[MAX_LENGTH], loaned_book[MAX_LENGTH];
        if (sscanf(line, "%[^:]:%[^\n]", loaned_username, loaned_book) == 2) {
            if (strcmp(username, loaned_username) == 0) {
                strcpy(loaned_books[loan_count], loaned_book);
                printf("[%d] %s\n", loan_count + 1, loaned_book);
                loan_count++;
            }
        }
    }
    fclose(loans);

    if (loan_count == 0) {
        printf("You have no books to return.\n");
        return;
    }

    int choice;
    printf("\nEnter the number of the book you want to return: ");
    scanf("%d", &choice);
    getchar(); // Consume newline character left in the buffer

    if (choice < 1 || choice > loan_count) {
        printf("Invalid choice. Please try again.\n");
        return;
    }

    char *book_title = loaned_books[choice - 1];

    loans = fopen("loans.txt", "r");
    if (!loans) {
        perror("Error reopening loans file");
        return;
    }

    FILE *temp = fopen("temp_loans.txt", "w");
    if (!temp) {
        perror("Error creating temporary file");
        fclose(loans);
        return;
    }

    while (fgets(line, sizeof(line), loans)) {
        trim_newline(line);

        char loaned_username[MAX_LENGTH], loaned_book[MAX_LENGTH];
        if (sscanf(line, "%[^:]:%[^\n]", loaned_username, loaned_book) == 2) {
            if (!(strcmp(username, loaned_username) == 0 && strcmp(book_title, loaned_book) == 0)) {
                fprintf(temp, "%s:%s\n", loaned_username, loaned_book);
            }
        }
    }

    fclose(loans);
    fclose(temp);

    remove("loans.txt");
    rename("temp_loans.txt", "loans.txt");

    FILE *books = fopen("books.txt", "a");
    if (!books) {
        perror("Error opening books file");
        return;
    }

    fprintf(books, "%s\n", book_title);
    fclose(books);

    printf("You have successfully returned '%s'.\n", book_title);
    system("cls");
}


int main(void) {
    char choice;
    int result;
    char username[MAX_LENGTH], password[MAX_LENGTH];
    int logged_in = 0;

    printf(" ===== ================== =====\n");
    printf(" ===== WELCOME TO LIBRARY =====\n");
    printf(" ===== ================== =====\n\n");

    while (1) {
        if (!logged_in) {
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
                        logged_in = 1;
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
        } else {
            printf("\n---- Library Menu ----\n");
            printf("[1]: List available books\n");
            printf("[2]: Borrow a book\n");
            printf("[3]: Return a book\n");
            printf("[Q]: Log out\n");
            printf("Please pick an option: ");

            scanf(" %c", &choice);
            getchar();

            choice = toupper(choice);

            switch (choice) {
                case '1':
                    list_books();
                    break;

                case '2':
                    borrow_book(username);
                    break;

                case '3':
                    return_book(username);
                    break;

                case 'Q':
                    printf("Logging out...\n");
                    logged_in = 0;
                    break;

                default:
                    printf("Invalid option. Please try again.\n");
                    break;
            }
        }
    }

    return 0;
}