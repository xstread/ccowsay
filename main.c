#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION "0.8.2"
#define MAX_WIDTH 50
#define COWS_DIR "/usr/local/share/ccowsay/cows"

void printBoxedText(const char *text);
char *combine_args(int argc, char *argv[]);
void searchAndDisplay(const char *cowName);
void printMessage();

int searchOrDisplay = 0;
int artFound;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printMessage();
    return 1;
  }

  searchAndDisplay(argv[1]);
  char *result = combine_args(argc, argv);

  if (result) {
    printBoxedText(result);
    free(result);
  }

  searchAndDisplay(argv[1]);
  return 0;
}

void searchAndDisplay(const char *cowName) {
  char filePath[256];

  if (strstr(cowName, ".cow") != NULL) {
    snprintf(filePath, sizeof(filePath), "%s/%s", COWS_DIR, cowName);
  } else {
    snprintf(filePath, sizeof(filePath), "%s/%s.cow", COWS_DIR, cowName);
  }

  FILE *file = fopen(filePath, "r");
  artFound = 1;
  if (!file) {
    artFound = 0;
    snprintf(filePath, sizeof(filePath), "%s/cow.cow", COWS_DIR);
    file = fopen(filePath, "r");
  }

  if (file) {
    char line[256];
    if (searchOrDisplay) {
      while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
      }
    }
    fclose(file);
  }
  searchOrDisplay++;
  printf("\n");
}

char *combine_args(int argc, char *argv[]) {
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
