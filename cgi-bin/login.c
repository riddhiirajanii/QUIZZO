#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#define MAX_LENGTH 100
#define TABLE_SIZE 100


typedef struct User {
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    struct User *next;
} User;

User *hashTable[TABLE_SIZE];

unsigned int hash(char *str) {
    unsigned int hash = 0;
    while (*str) {
        hash = (hash * 31) + *str;
        str++;
    }
    return hash % TABLE_SIZE;
}

// Function to generate a random session ID
void generatesessionID(char *sessionID) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    for (int i = 0; i < 20; i++) {
        sessionID[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    sessionID[20] = '\0';
}

// URL Decode function
void urlDecode(char *src, char *dest) {
    char *pSrc = src;
    char *pDest = dest;

    while (*pSrc) {
        if (*pSrc == '+') {
            *pDest = ' '; // Convert + to space
        } else if (*pSrc == '%' && isxdigit(*(pSrc + 1)) && isxdigit(*(pSrc + 2))) {
            // Convert %xx encoding
            char hex[3] = { *(pSrc + 1), *(pSrc + 2), '\0' };
            *pDest = (char)strtol(hex, NULL, 16);
            pSrc += 2;
        } else {
            *pDest = *pSrc;
        }
        pSrc++;
        pDest++;
    }
    *pDest = '\0';
}

// Get POST data
void getPostData(char *username, char *password) {
    char *lenstr;
    long len;
    char input[MAX_LENGTH * 2], decodedInput[MAX_LENGTH * 2];

    lenstr = getenv("CONTENT_LENGTH");
    if (lenstr == NULL || sscanf(lenstr, "%ld", &len) != 1 || len > sizeof(input) - 1) {
        printf("Content-type: text/html\n\n");
        printf("<html><body><h3>Invalid input.</h3></body></html>\n");
        exit(1);
    }

    fgets(input, len + 1, stdin);
    urlDecode(input, decodedInput);

    if (sscanf(decodedInput, "username=%99[^&]&password=%99s", username, password) != 2) {
        printf("Content-type: text/html\n\n");
        printf("<html><body><h3>Invalid input format.</h3></body></html>\n");
        exit(1);
    }
}

// Load users from file
void loadUsers() {
    FILE *file = fopen("users.txt", "r");
    if (!file) {
        printf("<p>Error: Cannot open users.txt</p>\n");
        exit(1);
    }

    char username[MAX_LENGTH], password[MAX_LENGTH];

    while (fscanf(file, "%s %s", username, password) != EOF) {
        unsigned int index = hash(username);

        User *newUser = (User *)malloc(sizeof(User));
        if (!newUser) {
            printf("Memory allocation error\n");
            fclose(file);
            exit(1);
        }
        
        strcpy(newUser->username, username);
        strcpy(newUser->password, password);
        newUser->next = hashTable[index];
        hashTable[index] = newUser;
    }

    fclose(file);
}

// Free allocated memory for users
void freeUsers() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        User *temp = hashTable[i];
        while (temp) {
            User *toFree = temp;
            temp = temp->next;
            free(toFree);
        }
        hashTable[i] = NULL;
    }
}

// Authenticate user
int authenticateUser(char *username, char *password) {
    unsigned int index = hash(username);
    User *temp = hashTable[index];
    
    while (temp) {
        if (strcmp(temp->username, username) == 0 && strcmp(temp->password, password) == 0) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

// Function to create session and redirect user
// Function to create session and redirect user
void createSession(char *username) {
    char sessionID[21];
    generatesessionID(sessionID);

    // Remove any existing session for this user
    FILE *tempFile = fopen("temp_sessions.txt", "w");
    FILE *sessionFile = fopen("sessions.txt", "r");

    if (sessionFile && tempFile) {
        char fileSessionID[MAX_LENGTH], fileUsername[MAX_LENGTH];

        while (fscanf(sessionFile, "%s %s", fileSessionID, fileUsername) != EOF) {
            if (strcmp(fileUsername, username) != 0) {
                fprintf(tempFile, "%s %s\n", fileSessionID, fileUsername);
            }
        }

        fclose(sessionFile);
        fclose(tempFile);
        remove("sessions.txt");
        rename("temp_sessions.txt", "sessions.txt");
    } else if (tempFile) {
        fclose(tempFile);
    }

    // Store new session
    sessionFile = fopen("sessions.txt", "a");
    if (!sessionFile) {
        printf("Content-type: text/html\n\n");
        printf("<html><body><h3>Error: Unable to store session.</h3></body></html>\n");
        exit(1);
    }

    fprintf(sessionFile, "%s %s\n", sessionID, username);
    fclose(sessionFile);

    // Send the response with JavaScript to set the cookie
    printf("Content-type: text/html\n\n");
    printf("<html><head>");
    printf("<script>");
    printf("document.cookie = 'sessionID=%s; Path=/; SameSite=Lax';", sessionID);
    printf("window.location.href = 'http://localhost/QUIZZO/www/dashboard.html';");
    printf("</script>");
    printf("</head><body><p>Redirecting...</p></body></html>\n");
}


// Function to validate session ID and retrieve the associated username
int validateSession(char *sessionID, char *username) {
    FILE *sessionFile = fopen("sessions.txt", "r");
    if (!sessionFile) {
        printf("<p>Error: Cannot open sessions.txt</p>\n");
        return 0;
    }

    char fileSessionID[MAX_LENGTH], fileUsername[MAX_LENGTH];
    while (fscanf(sessionFile, "%s %s", fileSessionID, fileUsername) != EOF) {
        if (strcmp(fileSessionID, sessionID) == 0) {
            strcpy(username, fileUsername); // Copy the associated username
            fclose(sessionFile);
            return 1; // Session is valid
        }
    }

    fclose(sessionFile);
    return 0; // Session is invalid
}

// Function to retrieve the session ID from the Cookie header
int getSessionIDFromCookie(char *sessionID) {
    char *cookie = getenv("HTTP_COOKIE");
    if (!cookie) {
        return 0; // No cookie found
    }

    char *sessionIDStart = strstr(cookie, "sessionID=");
    if (!sessionIDStart) {
        return 0; // sessionID not found in the cookie
    }

    sessionIDStart += strlen("sessionID="); // Move past "sessionID="
    char *sessionIDEnd = strchr(sessionIDStart, ';'); // Find the end of the session ID
    if (sessionIDEnd) {
        *sessionIDEnd = '\0'; // Null-terminate the session ID
    }

    strcpy(sessionID, sessionIDStart);
    return 1; // Session ID retrieved successfully
}

int main() {
    srand(time(NULL)); // Seed the random number generator

    char username[MAX_LENGTH], password[MAX_LENGTH];
    char sessionID[MAX_LENGTH];

    printf("Content-type: text/html\n\n");

    // Check if a valid session ID exists in the cookie
    if (getSessionIDFromCookie(sessionID)) {
        char sessionUser[MAX_LENGTH];
        if (validateSession(sessionID, sessionUser)) {
            // If a session exists but the user attempts a new login, remove the old session
            getPostData(username, password);
            if (strcmp(username, sessionUser) != 0) {
                FILE *tempFile = fopen("temp_sessions.txt", "w");
                FILE *sessionFile = fopen("sessions.txt", "r");

                if (sessionFile && tempFile) {
                    char fileSessionID[MAX_LENGTH], fileUsername[MAX_LENGTH];

                    while (fscanf(sessionFile, "%s %s", fileSessionID, fileUsername) != EOF) {
                        if (strcmp(fileSessionID, sessionID) != 0) {
                            fprintf(tempFile, "%s %s\n", fileSessionID, fileUsername);
                        }
                    }

                    fclose(sessionFile);
                    fclose(tempFile);
                    remove("sessions.txt");
                    rename("temp_sessions.txt", "sessions.txt");

                    // Clear the cookie
                    printf("Set-Cookie: sessionID=; Max-Age=0; Path=/;\r\n");
                }
            } else {
                // If the same user is logging in, just redirect to the dashboard
                printf("<html><head><meta http-equiv='refresh' content='0;url=http://localhost/QUIZZO/www/dashboard.html'></head>");
                printf("<body><p>Redirecting...</p></body></html>\n");
                return 0;
            }
        }
    }

    // If no valid session, proceed with login
    getPostData(username, password);
    loadUsers();

    if (authenticateUser(username, password)) {
        createSession(username);
    } else {
        printf("<html><body><h3>Invalid username or password.</h3></body></html>\n");
    }

    freeUsers(); // Clean up allocated memory
    return 0;
}
