#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define VERSION "0.9.4"
#define DEFAULT_WIDTH 40
#define MAX_LINES 256
#define COWS_DIR_INSTALLED "/usr/local/share/ccowsay/cows"
#define COWS_DIR_LOCAL "./cows"

typedef struct {
  char *cowfile;
  int wrap_width;
  int nowrap;
  int list_cows;
  char eyes[3];
  char tongue[3];
  int think_mode;
} Options;

FILE *searchCow(const char *cowName);
void printCow(FILE *file, const char *eyes, const char *tongue, int think_mode);
void printBalloon(char **lines, int lineCount, int maxLen, int think_mode);
char *combine_args(int argc, char *argv[], int start);
char *read_stdin();
void wrap_text(const char *text, char **lines, int *lineCount, int width);
void list_cowfiles();
void print_usage();
char *get_cowpath();
void apply_mode(Options *opts, char mode);
int is_cowthink(const char *argv0);

int main(int argc, char *argv[]) {
  Options opts = {.cowfile = "default",
                  .wrap_width = DEFAULT_WIDTH,
                  .nowrap = 0,
                  .list_cows = 0,
                  .eyes = "oo",
                  .tongue = "  ",
                  .think_mode = is_cowthink(argv[0])};

  char mode = 0;
  int opt;
  while ((opt = getopt(argc, argv, "f:W:e:T:bdgpstwynlh")) != -1) {
    switch (opt) {
    case 'f':
      opts.cowfile = optarg;
      break;
    case 'W':
      opts.wrap_width = atoi(optarg);
      if (opts.wrap_width <= 0)
        opts.wrap_width = DEFAULT_WIDTH;
      break;
    case 'e':
      strncpy(opts.eyes, optarg, 2);
      opts.eyes[2] = '\0';
      if (strlen(optarg) < 2) {
        opts.eyes[1] = opts.eyes[0];
      }
      break;
    case 'T':
      strncpy(opts.tongue, optarg, 2);
      opts.tongue[2] = '\0';
      if (strlen(optarg) < 2) {
        opts.tongue[1] = ' ';
      }
      break;
    case 'b':
    case 'd':
    case 'g':
    case 'p':
    case 's':
    case 't':
    case 'w':
    case 'y':
      mode = opt;
      break;
    case 'n':
      opts.nowrap = 1;
      break;
    case 'l':
      opts.list_cows = 1;
      break;
    case 'h':
      print_usage();
      return 0;
    default:
      print_usage();
      return 1;
    }
  }

  if (mode) {
    apply_mode(&opts, mode);
  }

  if (opts.list_cows) {
    list_cowfiles();
    return 0;
  }

  char *message = NULL;
  if (optind < argc) {
    message = combine_args(argc, argv, optind);
    if (!message) {
      fprintf(stderr, "Error: Memory allocation failed\n");
      return 1;
    }
  } else {
    message = read_stdin();
    if (!message) {
      fprintf(stderr, "Error: Memory allocation failed\n");
      return 1;
    }
  }

  if (!message || strlen(message) == 0) {
    print_usage();
    free(message);
    return 0;
  }

  char *lines[MAX_LINES];
  int lineCount = 0;
  int maxLen = 0;

  if (opts.nowrap) {
    char *line = strtok(message, "\n");
    while (line && lineCount < MAX_LINES) {
      lines[lineCount] = strdup(line);
      if (!lines[lineCount]) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        for (int i = 0; i < lineCount; i++) {
          free(lines[i]);
        }
        free(message);
        return 1;
      }
      int len = strlen(line);
      if (len > maxLen)
        maxLen = len;
      lineCount++;
      line = strtok(NULL, "\n");
    }
  } else {
    wrap_text(message, lines, &lineCount, opts.wrap_width);
    if (lineCount == 0 && strlen(message) > 0) {
      fprintf(stderr, "Error: Memory allocation failed during text wrapping\n");
      free(message);
      return 1;
    }
    for (int i = 0; i < lineCount; i++) {
      int len = strlen(lines[i]);
      if (len > maxLen)
        maxLen = len;
    }
  }

  printBalloon(lines, lineCount, maxLen, opts.think_mode);

  FILE *cowFile = searchCow(opts.cowfile);
  if (!cowFile) {
    fprintf(stderr, "Error: Could not find cow file '%s'\n", opts.cowfile);
    for (int i = 0; i < lineCount; i++) {
      free(lines[i]);
    }
    free(message);
    return 1;
  }

  printCow(cowFile, opts.eyes, opts.tongue, opts.think_mode);

  for (int i = 0; i < lineCount; i++) {
    free(lines[i]);
  }
  free(message);

  return 0;
}

void apply_mode(Options *opts, char mode) {
  switch (mode) {
  case 'b': // borg
    strcpy(opts->eyes, "==");
    break;
  case 'd': // dead
    strcpy(opts->eyes, "xx");
    strcpy(opts->tongue, "U ");
    break;
  case 'g': // greedy
    strcpy(opts->eyes, "$$");
    break;
  case 'p': // paranoid
    strcpy(opts->eyes, "@@");
    break;
  case 's': // stoned
    strcpy(opts->eyes, "**");
    strcpy(opts->tongue, "U ");
    break;
  case 't': // tired
    strcpy(opts->eyes, "--");
    break;
  case 'w': // wired
    strcpy(opts->eyes, "OO");
    break;
  case 'y': // young
    strcpy(opts->eyes, "..");
    break;
  }
}

int is_cowthink(const char *argv0) {
  if (!argv0)
    return 0;

  const char *basename = strrchr(argv0, '/');
  if (!basename)
    basename = argv0;
  else
    basename++;

  return strstr(basename, "cowthink") != NULL;
}

char *get_cowpath() {
  char *cowpath = getenv("COWPATH");
  if (cowpath)
    return cowpath;

  static char default_path[512];
  snprintf(default_path, sizeof(default_path), "%s:%s", COWS_DIR_INSTALLED,
           COWS_DIR_LOCAL);
  return default_path;
}

FILE *searchCow(const char *cowName) {
  char filePath[512];

  if (strchr(cowName, '/')) {
    FILE *file = fopen(cowName, "r");
    if (file)
      return file;
  }

  char *cowpath = get_cowpath();
  char *path_copy = strdup(cowpath);
  char *dir = strtok(path_copy, ":");

  while (dir) {
    if (strstr(cowName, ".cow")) {
      snprintf(filePath, sizeof(filePath), "%s/%s", dir, cowName);
    } else {
      snprintf(filePath, sizeof(filePath), "%s/%s.cow", dir, cowName);
    }

    FILE *file = fopen(filePath, "r");
    if (file) {
      free(path_copy);
      return file;
    }
    dir = strtok(NULL, ":");
  }

  free(path_copy);

  snprintf(filePath, sizeof(filePath), "%s/cow.cow", COWS_DIR_LOCAL);
  FILE *file = fopen(filePath, "r");
  if (file)
    return file;

  snprintf(filePath, sizeof(filePath), "%s/cow.cow", COWS_DIR_INSTALLED);
  return fopen(filePath, "r");
}

void printCow(FILE *file, const char *eyes, const char *tongue,
              int think_mode) {
  char thought = think_mode ? 'o' : '\\';

  char line[512];
  while (fgets(line, sizeof(line), file)) {
    // Calculate required output size
    size_t output_size =
        strlen(line) * 2 + 1; // Worst case: every char could expand
    char *output = malloc(output_size);
    if (!output) {
      fprintf(stderr, "Error: Memory allocation failed\n");
      fclose(file);
      return;
    }

    char *src = line;
    char *dst = output;

    while (*src) {
      // Check if we're getting close to buffer end
      if ((size_t)(dst - output) >= output_size - 20) {
        size_t current_pos = dst - output;
        output_size *= 2;
        char *new_output = realloc(output, output_size);
        if (!new_output) {
          free(output);
          fprintf(stderr, "Error: Memory reallocation failed\n");
          fclose(file);
          return;
        }
        output = new_output;
        dst = output + current_pos;
      }

      if (strncmp(src, "{{eyes}}", 8) == 0) {
        *dst++ = eyes[0];
        *dst++ = eyes[1];
        src += 8;
      } else if (strncmp(src, "{{tongue}}", 10) == 0) {
        *dst++ = tongue[0];
        *dst++ = tongue[1];
        src += 10;
      } else if (strncmp(src, "{{thoughts}}", 12) == 0) {
        *dst++ = thought;
        src += 12;
      } else {
        *dst++ = *src++;
      }
    }
    *dst = '\0';

    printf("%s", output);
    free(output);
  }
  fclose(file);
  printf("\n");
}

char *combine_args(int argc, char *argv[], int start) {
  size_t total_length = 0;
  for (int i = start; i < argc; i++) {
    total_length += strlen(argv[i]) + 1;
  }

  if (total_length == 0)
    return NULL;

  char *combined = malloc(total_length + 1);
  if (!combined) {
    perror("Failed to allocate memory");
    return NULL;
  }

  combined[0] = '\0';
  for (int i = start; i < argc; i++) {
    strcat(combined, argv[i]);
    if (i < argc - 1) {
      strcat(combined, " ");
    }
  }
  return combined;
}

char *read_stdin() {
  size_t size = 1024;
  size_t len = 0;
  char *buffer = malloc(size);

  if (!buffer) {
    perror("Failed to allocate memory");
    return NULL;
  }

  int c;
  while ((c = getchar()) != EOF) {
    if (len + 1 >= size) {
      size *= 2;
      char *new_buffer = realloc(buffer, size);
      if (!new_buffer) {
        free(buffer);
        perror("Failed to reallocate memory");
        return NULL;
      }
      buffer = new_buffer;
    }
    buffer[len++] = c;
  }

  buffer[len] = '\0';
  return buffer;
}

void wrap_text(const char *text, char **lines, int *lineCount, int width) {
  *lineCount = 0;
  char *text_copy = strdup(text);
  char *word = strtok(text_copy, " \t\n");

  size_t current_capacity = 256;
  char *current_line = malloc(current_capacity);
  if (!current_line) {
    free(text_copy);
    return;
  }
  current_line[0] = '\0';
  int current_len = 0;

  while (word && *lineCount < MAX_LINES) {
    int word_len = strlen(word);

    // Ensure buffer can hold current line + space + word + null terminator
    size_t needed = current_len + 1 + word_len + 1;
    if (needed > current_capacity) {
      current_capacity = needed * 2;
      char *new_line = realloc(current_line, current_capacity);
      if (!new_line) {
        free(current_line);
        free(text_copy);
        return;
      }
      current_line = new_line;
    }

    if (current_len == 0) {
      strcpy(current_line, word);
      current_len = word_len;
    } else if (current_len + 1 + word_len <= width) {
      strcat(current_line, " ");
      strcat(current_line, word);
      current_len += 1 + word_len;
    } else {
      lines[(*lineCount)++] = strdup(current_line);
      if (!lines[*lineCount - 1]) {
        free(current_line);
        free(text_copy);
        *lineCount = 0;
        return;
      }
      strcpy(current_line, word);
      current_len = word_len;
    }

    word = strtok(NULL, " \t\n");
  }

  if (current_len > 0 && *lineCount < MAX_LINES) {
    lines[(*lineCount)++] = strdup(current_line);
    if (!lines[*lineCount - 1]) {
      free(current_line);
      free(text_copy);
      *lineCount = 0;
      return;
    }
  }

  free(current_line);
  free(text_copy);
}

void printBalloon(char **lines, int lineCount, int maxLen, int think_mode) {
  if (lineCount == 0)
    return;

  printf(" ");
  for (int i = 0; i < maxLen + 2; i++)
    printf("_");
  printf(" \n");

  if (lineCount == 1) {
    printf("%c %s %c\n", think_mode ? '(' : '<', lines[0],
           think_mode ? ')' : '>');
  } else {
    for (int i = 0; i < lineCount; i++) {
      char left, right;
      if (i == 0) {
        left = think_mode ? '(' : '/';
        right = think_mode ? ')' : '\\';
      } else if (i == lineCount - 1) {
        left = think_mode ? '(' : '\\';
        right = think_mode ? ')' : '/';
      } else {
        left = think_mode ? '(' : '|';
        right = think_mode ? ')' : '|';
      }
      printf("%c %-*s %c\n", left, maxLen, lines[i], right);
    }
  }

  printf(" ");
  for (int i = 0; i < maxLen + 2; i++)
    printf("-");
  printf(" \n");
}

void list_cowfiles() {
  char *cowpath = get_cowpath();
  char *path_copy = strdup(cowpath);
  char *dir = strtok(path_copy, ":");
  int found_any = 0;

  while (dir) {
    DIR *d = opendir(dir);

    if (d) {
      struct dirent *entry;
      char *cows[256];
      int totalCows = 0;

      while ((entry = readdir(d)) != NULL && totalCows < 256) {
        if (strstr(entry->d_name, ".cow")) {
          cows[totalCows] = strdup(entry->d_name);
          if (cows[totalCows]) {
            char *dot = strstr(cows[totalCows], ".cow");
            if (dot)
              *dot = '\0';
            totalCows++;
          }
        }
      }
      closedir(d);

      if (totalCows > 0) {
        printf("Cow files in %s:\n", dir);
        for (int i = 0; i < totalCows; i++) {
          printf("%s ", cows[i]);
          free(cows[i]);
        }
        printf("\n");
        found_any = 1;
      } else {
        for (int i = 0; i < totalCows; i++) {
          free(cows[i]);
        }
      }
    }

    dir = strtok(NULL, ":");
  }

  free(path_copy);

  if (!found_any) {
    fprintf(stderr, "Error: No cow files found in COWPATH\n");
  }
}

void print_usage() {
  printf("ccowsay version %s\n", VERSION);
  printf("Usage: ccowsay [-f cowfile] [-W width] [-e eyes] [-T tongue] "
         "[-bdgpstwy] [-n] [-l] [-h] [message]\n\n");
  printf("Options:\n");
  printf("  -f cowfile    Specify cow file to use (default: default)\n");
  printf("  -W width      Set maximum width for word wrapping (default: %d)\n",
         DEFAULT_WIDTH);
  printf("  -e eyes       Set custom eyes (2 characters, default: oo)\n");
  printf("  -T tongue     Set custom tongue (2 characters, default: none)\n");
  printf("  -b            Borg mode (eyes: ==)\n");
  printf("  -d            Dead mode (eyes: xx, tongue: U )\n");
  printf("  -g            Greedy mode (eyes: $$)\n");
  printf("  -p            Paranoid mode (eyes: @@)\n");
  printf("  -s            Stoned mode (eyes: **, tongue: U )\n");
  printf("  -t            Tired mode (eyes: --)\n");
  printf("  -w            Wired mode (eyes: OO)\n");
  printf("  -y            Young mode (eyes: ..)\n");
  printf("  -n            Disable word wrapping\n");
  printf("  -l            List available cow files\n");
  printf("  -h            Show this help message\n\n");
  printf("If no message is provided, reads from stdin.\n");
  printf("Set COWPATH environment variable to add custom cow directories "
         "(colon-separated).\n");
  printf("\nNote: Mode flags (-bdgpstwy) override -e and -T options.\n");
}
