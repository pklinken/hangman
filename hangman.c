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

void init_str(char *str, char init_char, int len)
{
	for (int i = 0; i < len; i++)
	{
		str[i] = init_char;
	}
}

int has_char(char *str, char c)
{
	for (int i = 0; i < strlen(str); i++)
	{
		if (str[i] == c)
			return TRUE;
	}
	return FALSE;
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

	// printf("Your input: %c\n", in);

	return in;
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

void print_hangman(int error_count)
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

	printf("%s", hangman_copy);

	free(indices_to_blank);
	free(hangman_copy);
}

void print_word(char *word, char *guesses)
{
	char *output = xmalloc(sizeof(char) * strlen(word) - 1);
	for (int i = 0; i < strlen(word) - 1; i++)
	{
		output[i] = '_';
		for (int j = 0; j < strlen(guesses); j++)
		{
			if (word[i] == guesses[j])
				output[i] = word[i];
		}
	}
	output[strlen(word) - 1] = '\0';

	printf("Guess the word: %s\n", output);
	free(output);
}

int check_for_win(char *word, char *guesses)
{
	for (int i = 0; i < strlen(word) - 1; i++)
	{
		if (!has_char(guesses, word[i]))
			return FALSE;
	}

	return TRUE;
}

int main()
{
	int error_count = 0;

	char **word_list;
	int word_count;

	load_wordlist(&word_list, &word_count);

	// printf("Words available:\n");
	// for (int i = 0; i < word_count; i++)
	// {
	// 	printf("%i: %s", i + 1, word_list[i]);
	// }
	// printf("\n");

	srand(time(0));
	char *word = word_list[rand() % word_count];

	// printf("%s", word);

	char *guesses = xmalloc(sizeof(char) * 26);
	init_str(guesses, '\0', 26);

	int user_input;

	while (1)
	{
		print_hangman(error_count);
		print_word(word, guesses);
		if (strlen(guesses) > 0)
			printf("Previous guesses: %s\n", guesses);

		user_input = prompt_user();
		if (user_input == EOF)
		{
			printf("Error reading stdin, aborting");
			exit(1);
		}
		else
		{
			if (!isalpha((char)user_input))
				continue;

			if (has_char(guesses, (char)user_input))
			{
				printf("You already guessed this letter!");
				continue;
			}
			else
				guesses[strlen(guesses)] = (char)user_input;

			if (check_for_win(word, guesses))
			{
				printf("---¤¤ %s ¤¤--- Well done!!\n", word);
				return 0;
			}
			if (!has_char(word, (char)user_input))
			{
				error_count++;
			}
			if (error_count == 8)
			{
				print_hangman(error_count);
				printf("Game over! The answer was %s\n", word);
				return 0;
			}
		}
	}

	// Free words ??
	free(word_list);
	return 0;
}
