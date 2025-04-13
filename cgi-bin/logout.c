#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SESSION_FILE "sessions.txt"

int main() {
    printf("Content-type: text/html\n\n");

    char *cookie = getenv("HTTP_COOKIE");
    if (!cookie) {
        printf("No session to logout.");
        return 0;
    }

    char sessionID[100];
    char *start = strstr(cookie, "sessionID=");
    if (!start) {
        printf("No sessionID found.");
        return 0;
    }
    start += strlen("sessionID=");
    sscanf(start, "%99s", sessionID);

    FILE *file = fopen(SESSION_FILE, "r");
    FILE *temp = fopen("temp_sessions.txt", "w");

    char fileSessionID[100], username[100];
    while (fscanf(file, "%s %s", fileSessionID, username) != EOF) {
        if (strcmp(fileSessionID, sessionID) != 0) {
            fprintf(temp, "%s %s\n", fileSessionID, username);
        }
    }

    fclose(file);
    fclose(temp);

    remove(SESSION_FILE);
    rename("temp_sessions.txt", SESSION_FILE);

    printf("Logged out successfully.");
    return 0;
}
