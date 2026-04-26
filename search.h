#ifndef SEARCH_H
#define SEARCH_H
#include <stdio.h>
#include <string.h>
#include "vfs.h"
#include "str_utils.h"
#include "file_utils.h"
#include "journaling.h"

ArrayListVNodePtr get_matching_nodes_search(VNode *root, const char *search_term);
void find_nodes(VNode *node, const char *search_term, ArrayListVNodePtr *nodes);

ArrayListString get_all_paths_search(VNode *root, const char *search_term);
void find_paths(VNode *node, const char *search_term, ArrayListString *paths, ArrayListChar *current_path);
#endif
