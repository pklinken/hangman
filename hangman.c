#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_WORDS 1000

const char* hangman =
  " ____   \n"
  " |/ |   \n"
  " |  o   \n"
  " | /O\\  \n"
  " | / \\  \n"
  " |      \n"
  "--------\n";

void *
xmalloc (size_t size)
{
  void *value = malloc (size);
  if (value == 0) {
    printf("virtual memory exhausted");
    exit(1);
  }
  return value;
}

int prompt_user() {
  char* user_input;
  size_t n;

  int c, in;
  
  printf("Pick a letter: ");

  c = tolower(getchar());
  in = c;
  while(c != '\n' && c != EOF)
    c = getchar();
    
  printf("Your input: %c\n", in);
}

void load_wordlist(char*** result, int* wordcount) {
  char** lines = xmalloc(sizeof(char *) * MAX_WORDS);
  FILE* wordlist = fopen("./wordlist", "r");

  int line_count = 0;
  do {
    char* line = 0;
    size_t line_length = 0;
    
    if(getline(&line, &line_length, wordlist) < 0)
      break;

    lines[line_count++] = line;
  } while (line_count < MAX_WORDS);

  printf("Read %i lines from file\n", line_count);
  
  fclose(wordlist);
  
  *result = lines;
  *wordcount = line_count;
}

int main () {
  printf(hangman);
  return 0;
  
  char ** word_list;
  int word_count;
  
  load_wordlist(&word_list, &word_count);
  
  printf("Words available:\n");
  for(int i = 0; i < word_count; i++) {
    printf("%i: %s", i + 1, word_list[i]);
  }
  printf("\n");
  
  int user_input;
  
  while(1) {
    user_input = prompt_user();
    if(user_input == EOF) {
      printf("Error reading stdin, aborting");
      exit(1);
    }
    
  }

  // Free words ?? 
  return 0;
}
