#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TABLE_SIZE 100
#define MAX_LENGTH 100

typedef struct User {
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    int score;
    int rank;
    struct User *next;
} User;

User *hashTable[TABLE_SIZE]; 

unsigned int hash(const char *str) {
    unsigned int hash = 0;
    while (*str) {
        hash = (hash * 31) + *str;
        str++;
    }
    return hash % TABLE_SIZE;
}

void generateSessionID(char *sessionID) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < 20; i++) {
        sessionID[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    sessionID[20] = '\0'; 
}

int getSessionIDFromCookie(char *sessionID) {
    char *cookie = getenv("HTTP_COOKIE");
    if (cookie) {
        char *start = strstr(cookie, "sessionID=");
        if (start) {
            start += 10; 
            char *end = strchr(start, ';');
            if (end) {
                *end = '\0';
            }
            strcpy(sessionID, start);
            return 1;
        }
    }
    return 0;
}

void removeExistingSession(const char *username) {
    FILE *file = fopen("sessions.txt", "r");
    if (!file) return;

    FILE *tempFile = fopen("temp_sessions.txt", "w");
    if (!tempFile) {
        fclose(file);
        return;
    }

    char sessionID[MAX_LENGTH], storedUsername[MAX_LENGTH];
    int found = 0;

    while (fscanf(file, "%s %s", sessionID, storedUsername) == 2) {
        if (strcmp(storedUsername, username) != 0) {
            fprintf(tempFile, "%s %s\n", sessionID, storedUsername);
        } else {
            found = 1;
        }
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
        remove("sessions.txt");
        rename("temp_sessions.txt", "sessions.txt");
    }
}

void registerUser(const char *username, const char *password) {
    unsigned int index = hash(username);

    User *temp = hashTable[index];
    while (temp) {
        if (strcmp(temp->username, username) == 0) {
            printf("Content-type: text/html\n\n");
            printf("<html><body><h3>User already exists! Try a different one.</h3></body></html>\n");
            return;
        }
        temp = temp->next;
    }

    User *newUser = (User *)malloc(sizeof(User));
    if (!newUser) {
        printf("Content-type: text/html\n\n");
        printf("<html><body><h3>Memory allocation error!</h3></body></html>\n");
        return;
    }
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    newUser->score = 0;  
    newUser->rank = 0;   
    newUser->next = hashTable[index];
    hashTable[index] = newUser;

    FILE *file = fopen("C:/QUIZZO/cgi-bin/users.txt", "a");
    if (file) {
        fprintf(file, "%s %s %d %d\n", username, password, newUser->score, newUser->rank);
        fclose(file);
    } else {
        printf("Content-type: text/html\n\n");
        printf("<html><body><h3>Error saving user.</h3></body></html>\n");
        return;
    }

    char existingSessionID[MAX_LENGTH];
    if (getSessionIDFromCookie(existingSessionID)) {
        removeExistingSession(username);
        printf("Set-Cookie: sessionID=deleted; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT; HttpOnly; SameSite=Lax; Max-Age=0\r\n");
    }

    char sessionID[21];
    generateSessionID(sessionID);

    FILE *sessionFile = fopen("sessions.txt", "a");
    if (sessionFile) {
        fprintf(sessionFile, "%s %s\n", sessionID, username);
        fclose(sessionFile);

        printf("Content-type: text/html\n\n");
        printf("<html><head>");
        printf("<script>");
        printf("document.cookie = 'sessionID=%s; Path=/; SameSite=Lax';", sessionID);
        printf("window.location.href = 'http://localhost/QUIZZO/www/dashboard.html';");
        printf("</script>");
        printf("</head><body><p>Redirecting...</p></body></html>\n");
    } else {
        printf("Content-type: text/html\n\n");
        printf("<html><body><h3>Error: Unable to store session.</h3></body></html>\n");
    }
}

void urlDecode(const char *src, char *dest, size_t maxLen) {
    char hex[3];
    size_t i = 0;
    while (*src && i < maxLen - 1) {
        if (*src == '%') {
            hex[0] = *(src + 1);
            hex[1] = *(src + 2);
            hex[2] = '\0';
            *dest = (char)strtol(hex, NULL, 16);
            src += 3;
        } else if (*src == '+') {
            *dest = ' ';
            src++;
        } else {
            *dest = *src;
            src++;
        }
        dest++;
        i++;
    }
    *dest = '\0';
}

void getFormData(char *username, char *password, char *confirm_password) {
    char *lenstr;
    long len;
    char input[MAX_LENGTH * 4];
    char decodedInput[MAX_LENGTH * 4];

    lenstr = getenv("CONTENT_LENGTH");
    if (lenstr == NULL || sscanf(lenstr, "%ld", &len) != 1 || len > sizeof(input) - 1) {
        printf("Content-type: text/html\n\n");
        printf("<html><body><h3>Error: Invalid input</h3></body></html>\n");
        exit(1);
    }

    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Content-type: text/html\n\n");
        printf("<html><body><h3>Error reading input</h3></body></html>\n");
        exit(1);
    }

    urlDecode(input, decodedInput, sizeof(decodedInput));
    if (sscanf(decodedInput, "username=%99[^&]&email=%*[^&]&password=%99[^&]&confirm_password=%99[^&]",
               username, password, confirm_password) != 3) {
        printf("Content-type: text/html\n\n");
        printf("<html><body><h3>Error: Invalid form data</h3></body></html>\n");
        exit(1);
    }


    if (strcmp(password, confirm_password) != 0) {
        printf("Content-type: text/html\n\n");
        printf("<html><body><h3>Error: Passwords do not match</h3></body></html>\n");
        exit(1);
    }
}

int main() {
    srand(time(NULL)); // Initialize random seed

    printf("Content-type: text/html\n\n");  // CGI header

    char username[MAX_LENGTH], password[MAX_LENGTH], confirm_password[MAX_LENGTH];

    getFormData(username, password, confirm_password);
    registerUser(username, password);

    return 0;
}
