#include<string.h>
#include<stdio.h>

main()
{
	char a[5];
	char b[5];
	strcpy (a, "ciao\0");
	strcpy (b, "mond\0");
	printf("%s\n", strcat(a,b));
	
}	
