#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 256

int main() {
    printf("Content-type: application/json\n\n");

    char *cookie = getenv("HTTP_COOKIE");
    if (!cookie) {
        printf("{\"success\": false, \"message\": \"No cookie found\"}");
        return 1;
    }

    char *session = strstr(cookie, "sessionID=");
    if (!session) {
        printf("{\"success\": false, \"message\": \"Session not found\"}");
        return 1;
    }

    char sessionID[MAX], username[MAX];
    sscanf(session, "sessionID=%s", sessionID);

    FILE *sfile = fopen("sessions.txt", "r");
    if (!sfile) {
        printf("{\"success\": false, \"message\": \"Cannot open sessions.txt\"}");
        return 1;
    }

    int found = 0;
    while (fscanf(sfile, "%s %s", sessionID, username) != EOF) {
        if (strstr(cookie, sessionID)) {
            found = 1;
            break;
        }
    }
    fclose(sfile);

    if (!found) {
        printf("{\"success\": false, \"message\": \"Invalid session\"}");
        return 1;
    }

    FILE *ufile = fopen("users.txt", "r");
    char u[MAX], p[MAX], email[MAX], phone[MAX], course[MAX], rollnumber[MAX];
    int score, rank;

    while (fscanf(ufile, "%s %s %d %d %s %s %s %s", u, p, &score, &rank, email, phone, course, rollnumber) != EOF) {
        if (strcmp(u, username) == 0) {
            printf("{\"success\": true, \"username\": \"%s\", \"email\": \"%s\", \"phone\": \"%s\", \"course\": \"%s\", \"rollnumber\": \"%s\"}",
                   u, email, phone, course, rollnumber);
            fclose(ufile);
            return 0;
        }
    }

    fclose(ufile);
    printf("{\"success\": false, \"message\": \"User not found\"}");
    return 1;
}
