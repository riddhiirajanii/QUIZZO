#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 256

void decode_url(char *str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] == '+') str[i] = ' ';
    }
}

int main() {
    printf("Content-type: text/html\n\n");

    char *lenstr = getenv("CONTENT_LENGTH");
    if (!lenstr) {
        printf("<h3>Error: No data received</h3>");
        return 1;
    }

    int len = atoi(lenstr);
    char input[1024];
    fgets(input, len + 1, stdin);
    printf("<p>Raw input: %s</p>", input);
    decode_url(input);

    char old_username[MAX], username[MAX], password[MAX], email[MAX], phone[MAX], course[MAX], rollnumber[MAX];

    if (sscanf(input,
        "old_username=%[^&]&username=%[^&]&email=%[^&]&password=%[^&]&phone=%[^&]&course=%[^&]&rollnumber=%s",
        old_username, username, email, password, phone, course, rollnumber) != 7) {
        printf("<h3>Error: Invalid form data</h3>");
        printf("<p>old_username: %s</p>", old_username);
        printf("<p>username: %s</p>", username);
        printf("<p>email: %s</p>", email);
        printf("<p>password: %s</p>", password);
        printf("<p>phone: %s</p>", phone);
        printf("<p>course: %s</p>", course);
        printf("<p>rollnumber: %s</p>", rollnumber);
        
        return 1;
    }
    

    FILE *file = fopen("C:/xampp/cgi-bin/users.txt", "r");
    FILE *temp = fopen("C:/xampp/cgi-bin/temp.txt", "w");

    if (!file || !temp) {
        printf("<h3>Error opening files</h3>");
        return 1;
    }

    char line[1024];
    int updated = 0;

    while (fgets(line, sizeof(line), file)) {
        char u[MAX], p[MAX], e[MAX], ph[MAX], c[MAX], rnum[MAX];
        int score, rank;

        if (sscanf(line, "%s %s %d %d %s %s %s %s", u, p, &score, &rank, e, ph, c, rnum) == 8) {
            if (strcmp(u, old_username) == 0) {
                fprintf(temp, "%s %s %d %d %s %s %s %s\n",
                        username, password, score, rank, email, phone, course, rollnumber);
                updated = 1;
            } else {
                fputs(line, temp);
            }
        }
    }

    fclose(file);
    fclose(temp);

    // Update session mapping if username was changed
if (strcmp(old_username, username) != 0) {
    FILE *sessionFile = fopen("C:/xampp/cgi-bin/sessions.txt", "r");
    FILE *tempSession = fopen("C:/xampp/cgi-bin/temp_sessions.txt", "w");

    if (sessionFile && tempSession) {
        char sessionID[MAX], storedUsername[MAX];
        while (fscanf(sessionFile, "%s %s", sessionID, storedUsername) == 2) {
            if (strcmp(storedUsername, old_username) == 0) {
                fprintf(tempSession, "%s %s\n", sessionID, username);  // ✅ updated username
            } else {
                fprintf(tempSession, "%s %s\n", sessionID, storedUsername);
            }
        }
        fclose(sessionFile);
        fclose(tempSession);
        remove("C:/xampp/cgi-bin/sessions.txt");
        rename("C:/xampp/cgi-bin/temp_sessions.txt", "C:/xampp/cgi-bin/sessions.txt");
    }
}


    if (updated) {
        remove("C:/xampp/cgi-bin/users.txt");
        rename("C:/xampp/cgi-bin/temp.txt", "C:/xampp/cgi-bin/users.txt");
        printf("<html><head>");
        printf("<meta http-equiv='refresh' content='0;URL=/QUIZZO/www/dashboard.html'>");
        printf("</head><body>");
        printf("<p>Profile updated successfully! Redirecting...</p>");
        printf("</body></html>");

    } else {
        remove("C:/xampp/cgi-bin/temp.txt");
        printf("<h3 style='color:red;'>Username not found</h3><a href='/QUIZZO/edit_profile.html'>Try again</a>");
    }

    return 0;
}
