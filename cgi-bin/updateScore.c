#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 100


void updateScore(const char *username, int newScore) {
    FILE *file = fopen("users.txt", "r");
    if (!file) {
        printf("{\"success\": false, \"message\": \"Error opening users.txt\"}\n");
        return;
    }

    FILE *tempFile = fopen("users_temp.txt", "w");
    if (!tempFile) {
        fclose(file);
        printf("{\"success\": false, \"message\": \"Error creating temporary file\"}\n");
        return;
    }

    char fileUsername[MAX_LENGTH], filePassword[MAX_LENGTH];
    int fileScore, fileRank;
    int found = 0;

    while (fscanf(file, "%s %s %d %d", fileUsername, filePassword, &fileScore, &fileRank) == 4) {
        if (strcmp(fileUsername, username) == 0) {
            fileScore += newScore; 
            found = 1;
        }
        fprintf(tempFile, "%s %s %d %d\n", fileUsername, filePassword, fileScore, fileRank);
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
        if (remove("users.txt") != 0 || rename("users_temp.txt", "users.txt") != 0) {
            printf("{\"success\": false, \"message\": \"Error updating user record\"}\n");
            return;
        }
        printf("{\"success\": true, \"message\": \"Score successfully updated\"}\n");
    } else {
        remove("users_temp.txt"); // Cleanup
        printf("{\"success\": false, \"message\": \"User not found\"}\n");
    }
}

int main() {
    char *contentLength = getenv("CONTENT_LENGTH");
    if (!contentLength) {
        printf("Content-type: application/json\n\n");
        printf("{\"success\": false, \"message\": \"Missing input data\"}\n");
        return 1;
    }

    int len = atoi(contentLength);
    if (len <= 0 || len >= MAX_LENGTH) {
        printf("Content-type: application/json\n\n");
        printf("{\"success\": false, \"message\": \"Invalid input length\"}\n");
        return 1;
    }

    char input[MAX_LENGTH];
    if (!fgets(input, len + 1, stdin)) {
        printf("Content-type: application/json\n\n");
        printf("{\"success\": false, \"message\": \"Failed to read input\"}\n");
        return 1;
    }

    char username[MAX_LENGTH];
    int score;

    if (sscanf(input, "username=%99[^&]&score=%d", username, &score) != 2) {
        printf("Content-type: application/json\n\n");
        printf("{\"success\": false, \"message\": \"Invalid input format\"}\n");
        return 1;
    }

    printf("Content-type: application/json\n\n");
    updateScore(username, score);
    return 0;
}
