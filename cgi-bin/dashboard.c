#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 100

int getsessionID(char *sessionID) {
    char *cookie = getenv("HTTP_COOKIE");  
    if (!cookie) return 0;

    char *sessionidvalue = strstr(cookie, "sessionID=");
    if (!sessionidvalue) return 0;

    sessionidvalue += strlen("sessionID=");

    char *end = strchr(sessionidvalue, ';');
    if (end) {
        strncpy(sessionID, sessionidvalue, end - sessionidvalue);
        sessionID[end - sessionidvalue] = '\0'; 
    } else {
        strcpy(sessionID, sessionidvalue);
    }

    return 1;
}

void loadprofile(char *sessionID) {
    FILE *sessionFile = fopen("sessions.txt", "r");
    if (!sessionFile) {
        printf("Content-type: application/json\n\n");
        printf("{\"error\": \"Error accessing session database\"}\n");
        return;
    }

    char storedsessionid[50], username[MAX_LENGTH];
    int found = 0;
    while (fscanf(sessionFile, "%s %s", storedsessionid, username) != EOF) {
        if (strcmp(storedsessionid, sessionID) == 0) {
            found = 1;
            break;
        }
    }
    fclose(sessionFile);

    if (!found) {
        printf("Content-type: application/json\n\n");
        printf("{\"error\": \"No session found\"}\n");
        return;
    }

    FILE *userFile = fopen("users.txt", "r");
    if (!userFile) {
        printf("Content-type: application/json\n\n");
        printf("{\"error\": \"Error accessing user data\"}\n");
        return;
    }

    char fileUser[MAX_LENGTH], filePass[MAX_LENGTH];
    int score = 0, rank = 0; 
    while (fscanf(userFile, "%s %s %d %d", fileUser, filePass, &score, &rank) != EOF) {
        if (strcmp(fileUser, username) == 0) {
            break;  
        }
    }
    fclose(userFile);

    printf("Content-type: application/json\n\n");
    printf("{\"username\": \"%s\", \"score\": %d, \"rank\": %d}\n", username, score, rank);
}

int main() {
    char sessionID[50];

    if (getsessionID(sessionID)) {
        loadprofile(sessionID);
    } else {
        printf("Content-type: application/json\n\n");
        printf("{\"error\": \"No session cookie found\"}\n");
    }

    return 0;
}
