#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 100

int main() {
    char *contentlength = getenv("CONTENT_LENGTH");
    if (!contentlength) {
        printf("Content-type: application/json\n\n");
        printf("{\"success\": false, \"message\": \"No input received\"}\n");
        return 1;
    }

    int len = atoi(contentlength);
    if (len <= 0 || len >= MAX_LENGTH) {
        printf("Content-type: application/json\n\n");
        printf("{\"success\": false, \"message\": \"Input length is invalid.\"}\n");
        return 1;
    }

    char input[MAX_LENGTH];
    if (!fgets(input, len + 1, stdin)) {
        printf("Content-type: application/json\n\n");
        printf("{\"success\": false, \"message\": \"Unable to read input.\"}\n");
        return 1;
    }

    char sessionID[MAX_LENGTH];
    if (sscanf(input, "sessionID=%99s", sessionID) != 1) {
        printf("Content-type: application/json\n\n");
        printf("{\"success\": false, \"message\": \"Input format is invalid.\"}\n");
        return 1;
    }

    FILE *sessionfile = fopen("sessions.txt", "r");
    if (!sessionfile) {
        printf("Content-type: application/json\n\n");
        printf("{\"success\": false, \"message\": \"Unable to open sessions.txt\"}\n");
        return 1;
    }

    char filesessionID[MAX_LENGTH], fileUsername[MAX_LENGTH];
    while (fscanf(sessionfile, "%s %s", filesessionID, fileUsername) != EOF) {
        if (strcmp(filesessionID, sessionID) == 0) {
            fclose(sessionfile);
            printf("Content-type: application/json\n\n");
            printf("{\"success\": true, \"username\": \"%s\"}\n", fileUsername);
            return 0;
        }
    }

    fclose(sessionfile);
    printf("Content-type: application/json\n\n");
    printf("{\"success\": false, \"message\": \"Session ID not found\"}\n");
    return 1;
}