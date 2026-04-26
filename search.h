#include <stdio.h>
#include <string.h>
#include "star_compare.h"
#include "file_utils.h"
#include "journaling.h"
#include "vfs.h"
#define MAX_FILE_SIZE 1024

ArrayListVNodePtr get_matching_nodes_search(VNode *root, const char *search_term);
void find_nodes(VNode *node, const char *search_term, ArrayListVNodePtr *nodes);

ArrayListString get_all_paths_search(VNode *root, const char *search_term);
void find_paths(VNode *node, const char *search_term, ArrayListString *paths, ArrayListChar *current_path);