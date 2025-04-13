#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 100
#define MAX_LINE_LENGTH 256

typedef struct {
    char username[100];
    char password[100];
    int score;
    int rank;
    char email[100];
    char phonenumber[20];
    char course[100];
    char rollnumber[100];
} User;

int comparescores(const void *a, const void *b) {
    return ((User *)b)->score - ((User *)a)->score; // Sort in descending order
}

int main() {
    FILE* file = fopen("users.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    User users[MAX_USERS];
    int userCount = 0;
    char line[MAX_LINE_LENGTH];

    // Read users from file
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%s %s %d %d %s %s %s %s", users[userCount].username, users[userCount].password, &users[userCount].score, &users[userCount].rank, users[userCount].email, users[userCount].phonenumber, users[userCount].course, users[userCount].rollnumber) == 8) {
            userCount++;
        }
    }
    fclose(file);

    // Sort and update ranks
    qsort(users, userCount, sizeof(User), comparescores);
    for (int i = 0; i < userCount; i++) {
        users[i].rank = i + 1;
    }

    // Write updated data back to file
    file = fopen("users.txt", "w");
    if (file == NULL) {
        perror("Error writing to file");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < userCount; i++) {
        fprintf(file, "%s %s %d %d %s %s %s %s\n", users[i].username, users[i].password, users[i].score, users[i].rank, users[i].email, users[i].phonenumber, users[i].course, users[i].rollnumber);
    }

    fclose(file);

    // Print leaderboard JSON
    printf("Content-Type: application/json\n\n");
    printf("{\"leaderboard\": [");

    for (int i = 0; i < userCount; i++) {
        printf("{\"rank\": %d, \"name\": \"%s\", \"score\": %d}", users[i].rank, users[i].username, users[i].score);
        if (i < userCount - 1) {
            printf(", ");
        }
    }

    printf("]}");
    return 0;
}
