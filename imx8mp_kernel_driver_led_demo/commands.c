#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/virtio.h>
#include "rpmsg.h"

#define words_max_count 32
const char delimiter[] = " ";

static char *words[words_max_count];
static int words_count = 0;
static int parameters[words_max_count];
static int parameters_count = 0;

static void remove_last_r_or_n(char *word)
{
	int length = strlen(word);
	if (length > 0 && (word[length - 1] == '\r' || word[length - 1] == '\n'))
	{
		word[length - 1] = 0;
	}
}

static void remove_last_rn(char *word)
{
	remove_last_r_or_n(word);
	remove_last_r_or_n(word);
}

static bool parse_parameters(char *words[], int words_count)
{
	int i;
	long l;
	for (i = 1; i < words_count; i++)
	{
		if (kstrtol(words[i], 10, &l))
		{
			return false;
		}
		if (!(l >= INT_MIN && l <= INT_MAX))
		{
			return false;
		}
		parameters[parameters_count++] = (int)l;
	}

	return true;
}

static void execute_commands(const char *command)
{
	u8 led_r, led_g, led_b;
	enum led_mode_t led_mode;

	int command_length = strlen(command);
	if (command_length <= 0)
	{
		return;
	}

	if (strcmp("leds", command) == 0)
	{
		if (parameters_count != 3)
		{
			return;
		}

		led_r = parameters[0];
		led_g = parameters[1];
		led_b = parameters[2];
		led_mode = LED_MODE_MANUAL;
		rpmsg_enqueue_command_to_rpmsg(COMMAND_SEND_LEDS_STATES, led_r, led_g, led_b, led_mode);
		return;
	}

	if (strcmp("leds_mode", command) == 0)
	{
		if (parameters_count != 2)
		{
			led_mode = parameters[0];
			rpmsg_enqueue_command_to_rpmsg(COMMAND_SEND_LEDS_MODE, LED_COMMAND_UNDEFINED,
					LED_COMMAND_UNDEFINED, LED_COMMAND_UNDEFINED, led_mode);
			return;
		}

		led_mode = (enum led_mode_t)parameters[0];
		return;
	}
}

static void free_memory(void)
{
	int i;
	for (i = 0; i < words_count; i++)
	{
		kfree(words[i]);
	}
}

bool parse_line(char *line)
{
	int line_len;
	char *word_start;
	char *word_end;
	char *words_;
	words_count = 0;
	parameters_count = 0;

	line_len = strlen(line);
	line[line_len] = ' ';
	remove_last_rn(line);
	word_start = line;
	word_end = strchr(word_start, delimiter[0]);
	while (word_end != NULL)
	{
		*word_end = '\0';
		if (words_count + 1 >= words_max_count)
		{
			free_memory();
			return false;
		}

		words_ = (char*)kzalloc(1, strlen(word_start) + 1);
		if (words_ == NULL)
		{
			free_memory();
			return false;
		}

		strncpy(words_, word_start, strlen(word_start));
		words[words_count++] = words_;

		word_end++;
		word_start = word_end;
		word_end = strchr(word_start, delimiter[0]);
	}

	line[line_len] = '\0';

	// pr_info("words count %d", words_count);
	// for (i = 0; i < parameters_count; i++)
	// 	pr_info("word %d = %s", i, words[i]);

	if (words_count == 0 || !parse_parameters(words, words_count))
	{
		free_memory();
		return false;
	}

	// for (i = 0; i < parameters_count; i++)
	// 	pr_info("param %d = %d", i, parameters[i]);

	execute_commands(words[0]);
	free_memory();

	return true;
}
