#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <windows.h>

#define MAX_QUESTIONS 200
#define MAX_CHOICES 10
#define MAX_CATEGORIES 20

struct Choice {
    int correct;
    char letter;
    char text[200];
};

struct Question {
    int tag;
    int number;
    char text[666];
    struct Choice choices[MAX_CHOICES];
    int choiceCount;
};

struct Category {
    int tag;
    char name[200];
};

struct Category categories[MAX_CATEGORIES];
int categoryCount = 0;


int loadQuestions(struct Question questions[]) {
    FILE *fp = fopen("soal.txt", "r");
    if (!fp) {
        printf("File soal.txt tidak ditemukan!\n");
        return 0;
    }

    char line[666];
    int qCount = 0;

    struct Question q;
    memset(&q, 0, sizeof(q));

    int readingQuestion = 0;
    int readingChoices = 0;

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';

        // Skip blank lines, store previous question if needed
        if (strlen(line) == 0) {
            if (readingChoices) {
                questions[qCount++] = q;
                memset(&q, 0, sizeof(q));
                readingChoices = 0;
                readingQuestion = 0;
            }
            continue;
        }

        int tag, num;
        char rest[500];

        // ---------- CATEGORY ----------
        if (sscanf(line, "%d %d %[^\n]", &tag, &num, rest) == 3 && num == 0) {
            categories[categoryCount].tag = tag;
            strcpy(categories[categoryCount].name, rest);
            categoryCount++;
            continue;
        }

        // ---------- START OF NEW QUESTION ----------
        if (!readingQuestion && sscanf(line, "%d %d %[^\n]", &tag, &num, rest) == 3 && num != 0) {
            q.tag = tag;
            q.number = num;
            strcpy(q.text, rest);
            q.choiceCount = 0;
            readingQuestion = 1;

            // if ends with "\" continue next line
            if (q.text[strlen(q.text) - 1] == '\\') {
                continue;
            }

            readingChoices = 1;
            readingQuestion = 0;
            continue;
        }

        // ---------- MULTILINE QUESTION ----------
        if (readingQuestion) {
            strcat(q.text, "\n");
            strcat(q.text, line);

            if (q.text[strlen(q.text) - 1] == '\\') {
                continue;
            }

            readingChoices = 1;
            readingQuestion = 0;
            continue;
        }

        // ---------- CHOICES ----------
        if (readingChoices) {
            int correctFlag;
            char letter;
            char choiceText[300];

            if (sscanf(line, "%d %c %[^\n]", &correctFlag, &letter, choiceText) == 3) {
                q.choices[q.choiceCount].correct = correctFlag;
                q.choices[q.choiceCount].letter = letter;
                strcpy(q.choices[q.choiceCount].text, choiceText);
                q.choiceCount++;
            }
        }
    }

    // Save last question
    if (readingChoices) {
        questions[qCount++] = q;
    }

    fclose(fp);
    return qCount;
}
// ===============================================================


void shuffle(int *arr, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

int main() {
    struct Question questions[MAX_QUESTIONS];
    int total = loadQuestions(questions);
    if (total == 0) return 1;

    srand(time(NULL));
    char again;

    do {
        system("cls");
        printf("\n\n=== QUIZ GAME ===\n\n");

        // ======= LIST CATEGORIES ========
        printf("Kategori tersedia:\n");
        for (int i = 0; i < categoryCount; i++) {
            printf("%d. %s\n", categories[i].tag, categories[i].name);
        }

        int category;
        printf("Pilih kategori: ");
        scanf("%d", &category);

        // Filter questions by tag/category
        int indices[MAX_QUESTIONS];
        int count = 0;

        for (int i = 0; i < total; i++) {
            if (questions[i].tag == category) {
                indices[count++] = i;
            }
        }

        if (count == 0) {
            printf("Kategori kosong!\n");
            continue;
        }

        system("cls");

        shuffle(indices, count);
        int questionsToAsk = count < 5 ? count : 5;
        int score = 0;

        for (int i = 0; i < questionsToAsk; i++) {
            printf("\n\n");
            struct Question *q = &questions[indices[i]];

            printf("\nPertanyaan %d:\n%s\n", i + 1, q->text);
            for (int c = 0; c < q->choiceCount; c++) {
                printf("%c. %s\n", q->choices[c].letter, q->choices[c].text);
            }

            char ans;
            printf("Jawaban Anda (A/B/C/D): ");
            scanf(" %c", &ans);
            ans = toupper(ans);

            char correctLetter = '?';
            char correctText[300] = "";
            int correct = 0;

            for (int c = 0; c < q->choiceCount; c++) {
                if (q->choices[c].correct == 1) {
                    correctLetter = q->choices[c].letter;
                    strcpy(correctText, q->choices[c].text);
                }

                if (toupper(q->choices[c].letter) == ans &&
                    q->choices[c].correct == 1) {
                    correct = 1;
                }
            }

            if (correct) {
                printf("(V) Benar!\n");
                score++;
            } else {
                printf("(X) Salah! Jawaban yang benar: %c. %s\n",
                       correctLetter, correctText);
            }

            printf("===============");
        }

        printf("\n\nSkor Anda: %d/%d\n", score, questionsToAsk);

        printf("\nMain lagi? (y/n): ");
        scanf(" %c", &again);
        again = tolower(again);

    } while (again == 'y');

    system("cls");
    printf("\nTerima kasih sudah bermain!\n");
    return 0;
}
