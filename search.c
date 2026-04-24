#include "search.h"

DEF_ARRAY_LIST_SOURCE(char*, CharPtr, char_ptr);

ArrayListString get_all_paths_search(VNode *root, const char *search_term)
{
	ArrayListString paths;
	init_arr_list_str(&paths);

	ArrayListChar path_builder;
	init_arr_list_ch(&path_builder);

	search_ez_file(file, search_term, 0, &paths, &path_builder);

	free(path_builder.arr);
	return paths;
}

void search_ez_file(VNode *node, const char *search_term, ArrayListString *paths, ArrayListChar *current_path)
{
	int saved_len = current_path->length;

    if (current_path->length > 0) push_arr_list_ch(current_path, '/');
    for (int i = 0; node->name[i] != '\0'; i++)
    {
        push_arr_list_ch(current_path, node->name[i]);
    }

    if (star_compare(search_term, node->name))
    {
        // add null terminator for char* in paths
        push_arr_list_ch(current_path, '\0');

        // make a copy of current_path so we don't modify the value in the string array in other stack frames
        ArrayListChar copy;
        init_arr_list_ch(&copy);
        for (int i = 0; i < current_path->length; i++)
        {
            push_arr_list_ch(&copy, current_path->arr[i]);
        }
        push_arr_list_str(paths, copy);

        pop_arr_list_ch(current_path);
    }

    if (node->type == OBJ_DIRECTORY)
    {
        // recursively search all objects in directoruy
        for (int i = 0; i < node->dir.children.length i++)
        {
            search_vnode(node->dir.children.arr[i], search_term, paths, current_path);
        }
    }

    current_path->length = saved_len;
}