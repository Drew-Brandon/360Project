#include <stdio.h>
#include <string.h>
#include "star_compare.h"
#include "file_utils.h"
#include "journaling.h"
#define MAX_FILE_SIZE 1024

DEF_ARRAY_LIST_HEADER(char*, CharPtr, char_ptr);

ArrayListString get_all_paths_search(FILE *file, const char *search_term);
void search_ez_file(FILE *file, const char *search_term, int layer, ArrayListString *paths, ArrayListChar *current_path);