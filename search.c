#include "search.h"

DEF_ARRAY_LIST_SOURCE(char*, CharPtr, char_ptr);

ArrayListString get_all_paths_search(FILE *file, const char *search_term)
{
	ArrayListString paths;
	init_arr_list_str(&paths);

	ArrayListChar path_builder;
	init_arr_list_ch(&path_builder);

	search_ez_file(file, search_term, 0, &paths, &path_builder);

	for (int i = 0; i < paths.length; i++)
	{
		printf("%s\n", paths.arr[i].arr);
	}

	free(path_builder.arr);
	return paths;
}

void search_ez_file(FILE *file, const char *search_term, int layer, ArrayListString *paths, ArrayListChar *current_path)
{
	char obj_name[MAX_FILE_NAME_SIZE];

    while (1)
    {
        skip_whitespace(file);
        
        if (peek_char(file) == '}' || peek_char(file) == EOF)
        {
            fgetc(file);
            break;
        }

        JCALL(enum EZObjectType obj_type = scan_header(file, MAX_FILE_NAME_SIZE, obj_name));

        // append current object to current_path
        int saved_len = current_path->length;
        if (current_path->length > 0) push_arr_list_ch(current_path, '/');
        for (int i = 0; obj_name[i] != '\0'; i++)
		{
		    push_arr_list_ch(current_path, obj_name[i]);
		}

        switch (obj_type)
        {
            case OBJ_DIRECTORY:
            	skip_to_char(file, '{');
    			fgetc(file);
    			//char *recurse_path = strdup(new_path);
                if (star_compare(search_term, obj_name)) 
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

                search_ez_file(file, search_term, layer + 1, paths, current_path);
                break;
            case OBJ_FILE:
            	skip_to_char(file, '\"');
			    JCALL(scan_till_char(file, '\"', layer + 1));
			    fgetc(file);
                if (star_compare(search_term, obj_name))
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
                break;
        }
        current_path->length = saved_len;
    }
}