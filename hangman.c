#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_WORDS 1000
#define MAX_ERROR_COUNT 8

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

bool has_char(char *str, char c)
{
	for (int i = 0; i < strlen(str); i++)
	{
		if (str[i] == c)
			return true;
	}
	return false;
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

struct WordList
{
	char **words;
	size_t len;
};

struct WordList load_wordlist()
{
	struct WordList result;
	result.words = xmalloc(sizeof(char *) * MAX_WORDS);
	result.len = 0;

	FILE *f_wordlist = fopen("./wordlist", "r");

	do
	{
		char *line = 0;
		size_t line_length = 0;

		if (getline(&line, &line_length, f_wordlist) < 0)
			break;

		result.words[result.len++] = line;
	} while (result.len < MAX_WORDS);

	printf("Read %lu lines from file\n", result.len);

	fclose(f_wordlist);

	return result;
}

// Cells in the hangman gfx to blank
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

void get_blank_indices(int error_count, int **indices, int *count)
{
	*indices = xmalloc(sizeof(int) * 8 * 6);
	*count = 0;
	int level = 7 - error_count;

	while (level >= 0)
	{
		int level_index = 0;
		while (levels[level][level_index] != -1)
		{
			(*indices)[*count] = levels[level][level_index];
			(*count)++;
			level_index++;
		}

		level--;
	}
}

void print_hangman(int error_count)
{
	char *hangman_copy = xmalloc(strlen(hangman) + 1);
	strcpy(hangman_copy, hangman);

	int *indices_to_blank, count;
	get_blank_indices(error_count, &indices_to_blank, &count);

	while (count > 0)
	{
		hangman_copy[indices_to_blank[count - 1]] = ' ';
		count--;
	}

	printf("%s", hangman_copy);

	free(indices_to_blank);
	free(hangman_copy);
}

void print_word(char *word, char *guesses)
{
	char *output = xmalloc(strlen(word) + 1);
	for (int i = 0; i < strlen(word); i++)
		output[i] = has_char(guesses, word[i]) ? word[i] : '_';

	output[strlen(word)] = '\0';

	printf("Guess the word: %s\n", output);
	free(output);
}

bool check_for_win(char *word, char *guesses)
{
	for (int i = 0; i < strlen(word); i++)
	{
		if (!has_char(guesses, word[i]))
			return false;
	}

	return true;
}

int main()
{
	// get wordlist
	struct WordList wordlist = load_wordlist();

	// set random seed and pick random word
	srand(time(0));
	int random_index = rand() % wordlist.len;
	// printf("%i", strlen(wordlist.words[random_index]));
	char *word_to_guess = xmalloc(strlen(wordlist.words[random_index]));
	// Copy to new buffer and strip the \n
	// The string from getline would be "abc\n" so strlen() returns 4 but the buffer length is 5
	strncpy(word_to_guess, wordlist.words[random_index], strlen(wordlist.words[random_index]) - 1);
	word_to_guess[strlen(wordlist.words[random_index]) - 1] = '\0';

	// Free the memory we allocated so far
	for (int i = 0; i < wordlist.len; i++)
		free(wordlist.words[i]);
	free(wordlist.words);

	// Up to 26 guesses
	char *guesses = xmalloc(sizeof(char) * 27);
	init_str(guesses, '\0', 27);

	int user_input;
	int error_count = 0;
	while (true)
	{
		if (error_count > 0)
			print_hangman(error_count);
		print_word(word_to_guess, guesses);
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
				printf("You already guessed this letter!\n");
				continue;
			}
			else
				guesses[strlen(guesses)] = (char)user_input;

			if (check_for_win(word_to_guess, guesses))
			{
				printf("---¤¤ %s ¤¤---\nWell done!!\n", word_to_guess);
				return 0;
			}

			if (!has_char(word_to_guess, (char)user_input))
				error_count++;

			if (error_count == MAX_ERROR_COUNT)
			{
				print_hangman(MAX_ERROR_COUNT);
				printf("Game over! The answer was %s\n", word_to_guess);
				return 0;
			}
		}
	}

	return 0;
}
