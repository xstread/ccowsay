#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define VERSION "0.9.4"
#define MAX_WIDTH 50
#define COWS_DIR "/usr/local/share/ccowsay/cows"


FILE *searchCow(const char *cowName, int *artPTR);
void printCow(FILE *file);
void printBoxedText(const char *text);
char *combine_args(int argc, char *argv[], int artFound);
void printMessage();

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printMessage();
    return 0;
  }

  int artFound = 1;
  int *artPTR = &artFound;
  FILE *cowFile = searchCow(argv[1], artPTR);
  char *result = combine_args(argc, argv, artFound);

  if (result) {
    printBoxedText(result);
    free(result);
  }

  printCow(cowFile);
  return 0;
}

FILE *searchCow(const char *cowName, int *artPTR) {
  char filePath[256];
  
  //Error handling for filePath
  if (strlen(COWS_DIR) + strlen(cowName) + 1 >= sizeof(filePath)) {
    fprintf(stderr, "File path exceeds buffer size\n");
    return NULL;
  }

  if (strstr(cowName, ".cow") != NULL) {
    snprintf(filePath, sizeof(filePath), "%s/%s", COWS_DIR, cowName);
  } else {
    snprintf(filePath, sizeof(filePath), "%s/%s.cow", COWS_DIR, cowName);
  }

  FILE *file = fopen(filePath, "r");
  if (!file) {
    snprintf(filePath, sizeof(filePath), "%s/cow.cow", COWS_DIR);
    file = fopen(filePath, "r");
    *artPTR = 0;
    return file;
  }
  file = fopen(filePath, "r");
  return file;
}

void printCow(FILE *file) {
  char line[256];
  while (fgets(line, sizeof(line), file)) {
    printf("%s", line);
  }
  fclose(file);
  printf("\n");
}

char *combine_args(int argc, char *argv[], int artFound) {
  size_t total_length = 0;
  for (int i = artFound + 1; i < argc; i++) {
    total_length += strlen(argv[i]) + 1; // +1 for space
  }

  char *combined = malloc(total_length + 1); // +1 for the null terminator
  if (combined == NULL) {
    perror("Failed to allocate memory");
    return NULL;
  }

  combined[0] = '\0';
  for (int i = artFound + 1; i < argc; i++) {

    strcat(combined, argv[i]);
    if (i < argc - 1) {
      strcat(combined, " ");
    }
  }
  return combined;
}

void printBoxedText(const char *text) {
  if (text == NULL || strlen(text) == 0) {
    printMessage();
    exit(0);
  }

  int length = strlen(text);
  int boxWidth = length < MAX_WIDTH ? length : MAX_WIDTH;
  int start = 0;

  printf("+");
  for (int i = 0; i < boxWidth + 2; i++) {
    printf("-");
  }
  printf("+\n");
  int numLines = (length + boxWidth - 1) / boxWidth;
  for (int i = 0; i < numLines; i++) {
    printf("| ");

    for (int j = 0; j < boxWidth; j++) {
      if (start + j < length) {
        printf("%c", text[start + j]);
      } else {
        printf(" ");
      }
    }

    printf(" |\n");
    start += boxWidth;
  }
  printf("+");

  for (int i = 0; i < boxWidth + 2; i++) {
    printf("-");
  }
  printf("+\n");
}

void printMessage() {
  printf("ccowsay version %s\n\nUsage: \n\t\tccowsay <ascii-art> <message>\n\n",
         VERSION);

  printf("Possible ASCII Art values:\n");

  DIR *dir = opendir(COWS_DIR);
  if (dir == NULL) {
    perror("Unable to open cows directory");
    return;
  }

  struct dirent *entry;
  int count = 0;
  int columnCount = 3;
  char *cows[256];
  int totalCows = 0;

  while ((entry = readdir(dir)) != NULL) {
    if (strstr(entry->d_name, ".cow") != NULL) {
      cows[totalCows++] = strdup(entry->d_name);
      if (cows[totalCows - 1] == NULL) {
        perror("Failed to allocate memory for cow name");
        closedir(dir);
        return;
      }
    }
  }
  closedir(dir);

  for (int i = 0; i < totalCows; i++) {
    printf("%-*.*s", 20, (int)(strlen(cows[i]) - 4), cows[i]);
    count++;
    if (count % columnCount == 0) {
      printf("\n");
    }
  }
  if (count % columnCount != 0) {
    printf("\n");
  }

  for (int i = 0; i < totalCows; i++) {
    free(cows[i]);
  }
  printf("\n");
}
