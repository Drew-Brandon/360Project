#include "star_parse.h"

int star_contains(const char* search_term, const char* searched_text)
{
	if (*search_term == '*')
	{
		// skip * until new character
		while (*search_term == '*') search_term++;

		// if * is at the end and previous part of the string is matching, everything else matches
		if (*search_term == '\0') return 1;

		while (*searched_text != '\0')
		{
			// recursively check matches in searched_text
			if (*searched_text == *search_term && star_contains(search_term, searched_text)) return 1;
			searched_text++;
		}

		// return false if we reach the end of the searched text without returning true early
		return 0;
	}

	// if we are at the end of both strings, return true
	if (*search_term == '\0' && *searched_text == '\0') return 1;

	// if we reach the end of only one string, return false
	if (*search_term == '\0' || *searched_text == '\0') return 0;

	// check current char match if we are not at the end of either string
	if (*search_term != *searched_text) return 0;

	return star_contains(search_term + 1, searched_text + 1);
}