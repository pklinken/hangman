#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORDS 1000
#define TRUE 1
#define FALSE 0

const char hangman[] =
	" ____   \n"
	" |/ |   \n"
	" |  o   \n"
	" | /O\\  \n"
	" | / \\  \n"
	" |      \n"
	"--------\n";

void *xmalloc(size_t size)
{
	void *value = malloc(size);
	if (value == 0)
	{
		printf("virtual memory exhausted");
		exit(1);
	}
	return value;
}

int prompt_user()
{
	char *user_input;
	size_t n;

	int c, in;

	printf("Pick a letter: ");

	c = tolower(getchar());
	in = c;
	while (c != '\n' && c != EOF)
		c = getchar();

	printf("Your input: %c\n", in);
}

void load_wordlist(char ***result, int *wordcount)
{
	char **lines = xmalloc(sizeof(char *) * MAX_WORDS);
	FILE *wordlist = fopen("./wordlist", "r");

	int line_count = 0;
	do
	{
		char *line = 0;
		size_t line_length = 0;

		if (getline(&line, &line_length, wordlist) < 0)
			break;

		lines[line_count++] = line;
	} while (line_count < MAX_WORDS);

	printf("Read %i lines from file\n", line_count);

	fclose(wordlist);

	*result = lines;
	*wordcount = line_count;
}

int levels[8][6] = {
	{41, -1},
	{39, -1},
	{32, -1},
	{30, -1},
	{31, -1},
	{13, 22, -1},
	{1, 2, 3, 4, 11, -1},
	{10, 19, 28, 37, 46, -1},
};

int *get_blank_indices(int error_count)
{
	int *indices = xmalloc(sizeof(int) * 8 * 6);
	int i = 0;
	int level = 7 - error_count;

	while (level >= 0)
	{
		int level_index = 0;
		while (levels[level][level_index] != -1)
		{
			indices[i] = levels[level][level_index];
			i++;
			level_index++;
		}

		level--;
	}

	indices[i] = -1;

	return indices;
}

char *gen_drawing(int error_count)
{
	char *hangman_copy = xmalloc(strlen(hangman) + 1);
	strcpy(hangman_copy, hangman);

	int *indices_to_blank = get_blank_indices(error_count);

	int i = 0;
	while (indices_to_blank[i] != -1)
	{
		hangman_copy[indices_to_blank[i]] = ' ';
		i++;
	}

	free(indices_to_blank);

	return hangman_copy;
}

int main()
{
	printf(gen_drawing(0));
	printf(gen_drawing(1));
	printf(gen_drawing(2));
	printf(gen_drawing(3));
	printf(gen_drawing(4));
	printf(gen_drawing(5));
	printf(gen_drawing(6));
	printf(gen_drawing(7));
	printf(gen_drawing(8));

	// int error_count = 1;

	// char *drawing = gen_drawing(error_count);
	// printf(drawing);
	return 0;

	char **word_list;
	int word_count;

	load_wordlist(&word_list, &word_count);

	printf("Words available:\n");
	for (int i = 0; i < word_count; i++)
	{
		printf("%i: %s", i + 1, word_list[i]);
	}
	printf("\n");

	int user_input;

	while (1)
	{
		user_input = prompt_user();
		if (user_input == EOF)
		{
			printf("Error reading stdin, aborting");
			exit(1);
		}
	}

	// Free words ??
	free(word_list);
	return 0;
}
