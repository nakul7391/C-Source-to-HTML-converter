/*Main function for source to html*/
#include <stdio.h>
#include <string.h>
#include "Global.h"
#include "html_conv.h"

int main(int argc, char *argv[] )
{
		/*check arguments*/
		if( argc < 2 )
		{
				printf("Invalid arguments\n");
				printf("./a.out --help for usage\n");
				return 1;
		}

		/*check help*/
		if( strcmp(argv[1],"--help") == 0 )
		{
				printf("./a.out <file.c> <file.html>\n");
				return 2;
		}

		/*open sample file and destination file*/
		fptr_src = fopen(argv[1],"r");
		fptr_dest = fopen(argv[2],"w");

		if( fptr_src == NULL || fptr_dest == NULL )
		{
				perror("");
				printf("Unable to open files\n");
				return 3;
		}

		/*insert html header*/
		insert_header();

		/*html conversion begin*/
		html_conv();
		/*insert html footer*/
		insert_footer();

		/*close the files*/
		fclose(fptr_src);
		fclose(fptr_dest);

		return 0;
}
