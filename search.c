#include "search.h"

ArrayListVNodePtr get_matching_nodes_search(VNode *root, const char *search_term)
{
    ArrayListVNodePtr nodes;
    init_arr_list_vnode_ptr(&nodes);

    find_nodes(root, search_term, &nodes);

    return nodes;
}

void find_nodes(VNode *node, const char *search_term, ArrayListVNodePtr *nodes)
{
    // if we find a match, add it to the list
    if (star_compare(search_term, node->name))
    {
        push_arr_list_vnode_ptr(nodes, node);
    }

    if (node->type == OBJ_DIRECTORY)
    {
        // recursively search all objects in directory
        for (int i = 0; i < node->dir.children.length; i++)
        {
            find_nodes(node->dir.children.arr[i], search_term, nodes);
        }
    }
}

ArrayListString get_all_paths_search(VNode *root, const char *search_term)
{
	ArrayListString paths;
	init_arr_list_str(&paths);

	ArrayListChar path_builder;
	init_arr_list_ch(&path_builder);

	find_paths(root, search_term, &paths, &path_builder);

	free(path_builder.arr);
	return paths;
}

void find_paths(VNode *node, const char *search_term, ArrayListString *paths, ArrayListChar *current_path)
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
        // recursively search all objects in directory
        for (int i = 0; i < node->dir.children.length; i++)
        {
            find_paths(node->dir.children.arr[i], search_term, paths, current_path);
        }
    }

    current_path->length = saved_len;
}