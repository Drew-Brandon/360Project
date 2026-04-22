#include "search.h"

int search_ez_file(FILE *file, const char *search_term, int layer)
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

        skip_whitespace(file);
        JCALL(enum EZObjectType obj_type = scan_header(file, MAX_FILE_NAME_SIZE, obj_name));

        switch (obj_type)
        {
            case OBJ_DIRECTORY:
                if (star_compare(search_term, obj_name))
                { 
                	printf("Found directory match: %s \n", obj_name);
                	return 1;
                }
                int found;
    			JCALL(found = search_dir(file, search_term, layer));
    			if (found) return 1;
                break;
            case OBJ_FILE:
                if (star_compare(search_term, obj_name)) 
                {
                	printf("Found file match: %s \n", obj_name);
                	return 1;
                }
                search_file(file, search_term, layer);
                break;
        }
    }

    return 0;
}

int search_dir(FILE *file, const char *search_term, int layer)
{
    skip_to_char(file, '{');
    fgetc(file);
    int result;
    JCALL(result = search_ez_file(file, search_term, layer + 1));
    return result;
}

void search_file(FILE *file, const char *search_term, int layer)
{
    skip_to_char(file, '\"');
    JCALL(scan_till_char(file, '\"', layer + 1));
    fgetc(file);
}