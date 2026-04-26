#include <string.h>
#include "str_utils.h"

DEF_ARRAY_LIST_SOURCE(char, Char, ch)
DEF_ARRAY_LIST_SOURCE(char *, String, str)
DEF_ARRAY_LIST_SOURCE(Line, Line, line)

void free_str_list(ArrayListString *list)
{
	for (int i = 0; i < list->length; i++)
	{
		free(list->arr[i]);
	}

	free(list->arr);
}

boolean star_compare(const char* search_term, const char* searched_text)
{
	if (*search_term == '*')
	{
		// skip * until new character
		while (*search_term == '*') search_term++;

		// if * is at the end and previous part of the string is matching, everything else matches
		if (*search_term == '\0') return TRUE;

		while (*searched_text != '\0')
		{
			// recursively check matches in searched_text
			// it is required to search each character if it matches the char following the * for cases like this:
			//		search_term: 	a*ba
			//		searched_text:	abcba
			//
			// if we only searched for the first appearance of search_term's char after the * (b), we would continue
			// comparing and return false after bc because a != c, but if we continue until the next appearance of a,
			// we can find ba which will eventually return true as they both reach the end of their strings
			if (*searched_text == *search_term && star_compare(search_term, searched_text)) return 1;
			searched_text++;
		}

		// return false if we reach the end of the searched text without returning true early
		return FALSE;
	}

	// if we are at the end of both strings, they are equal, return true
	if (*search_term == '\0' && *searched_text == '\0') return 1;

	// if we reach the end of only one string, they are not equal, return false
	if (*search_term == '\0' || *searched_text == '\0') return 0;

	// check current char match if we are not at the end of either string
	if (*search_term != *searched_text) return 0;

	return star_compare(search_term + 1, searched_text + 1);
}

ArrayListString split_str(char *str, char delimiter)
{
	int i = 0;
	ArrayListChar cur_token;
	ArrayListString tokens;
	init_arr_list_ch(&cur_token);
	init_arr_list_str(&tokens);

	while (str[i])
	{
		if (str[i] == delimiter)
		{
			push_arr_list_ch(&cur_token, '\0');
			pack_arr_list_ch(&cur_token);
			push_arr_list_str(&tokens, cur_token.arr);
			init_arr_list_ch(&cur_token);
		}
		else
		{
			push_arr_list_ch(&cur_token, str[i]);
		}

		i++;
	}

	push_arr_list_ch(&cur_token, '\0');
	pack_arr_list_ch(&cur_token);
	push_arr_list_str(&tokens, cur_token.arr);
	pack_arr_list_str(&tokens);
	return tokens;
}

ArrayListChar copy_str_till(char *str, char stop)
{
	int i = 0;
	ArrayListChar copy;
	init_arr_list_ch(&copy);

	while (str[i] && str[i] != stop)
	{
		push_arr_list_ch(&copy, str[i]);
		i++;
	}

	push_arr_list_ch(&copy, '\0');
	pack_arr_list_ch(&copy);
	return copy;
}
