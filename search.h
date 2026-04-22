#include <stdio.h>
#include "star_compare.h"
#include "file_utils.h"
#include "journaling.h"

int search_ez_file(FILE *file, const char *search_term, int layer);
int search_dir(FILE *file, const char *search_term, int layer);
void search_file(FILE *file, const char *search_term, int layer);