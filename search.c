#include "search.h"

DEF_ARRAY_LIST_SOURCE(char*, CharPtr, char_ptr);

void get_all_paths_search(FILE *file, const char *search_term)
{
	ArrayListCharPtr paths;
	init_arr_list_char_ptr(&paths);

	search_ez_file(file, search_term, 0, &paths, "");

	for (int i = 0; i < paths.length; i++)
	{
		printf("%s\n", paths.arr[i]);
	}
}

void search_ez_file(FILE *file, const char *search_term, int layer, ArrayListCharPtr *paths, const char *current_path)
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

        char *new_path = malloc(strlen(current_path) + strlen(obj_name) + 2);
        strcpy(new_path, current_path);
        if (strlen(current_path) > 0) strcat(new_path, "/");
        strcat(new_path, obj_name);

        switch (obj_type)
        {
            case OBJ_DIRECTORY:
            	skip_to_char(file, '{');
    			fgetc(file);
    			char *recurse_path = strdup(new_path);
                if (star_compare(search_term, obj_name)) 
                {
                	push_arr_list_char_ptr(paths, new_path);
					new_path = NULL;
                }

                search_ez_file(file, search_term, layer + 1, paths, recurse_path);
                free(recurse_path);
                break;
            case OBJ_FILE:
            	skip_to_char(file, '\"');
			    JCALL(scan_till_char(file, '\"', layer + 1));
			    fgetc(file);
                if (star_compare(search_term, obj_name))
                {
                	push_arr_list_char_ptr(paths, new_path);
                	new_path = NULL;
                }
                break;
        }
        if (new_path) free(new_path);
    }
}