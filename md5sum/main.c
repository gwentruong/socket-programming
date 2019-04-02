#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>
#include "md5.h"

int main (int argc, char **argv)
{
	struct MD5Context context;
	unsigned char checksum[16];
    FILE *fp;
	int i;
	int j;
    int size;

	if (argc < 2)
	{
		fprintf (stderr, "Usage: %s <file> [<file> [...] ]\n", argv[0]);
		exit(1);
	}
    
	for (j = 1; j < argc; ++j)
	{
        fp = fopen(argv[j], "r");
        if (fp == NULL)
        {
            printf("File %s can't be opened\n", argv[j]);
            exit(1);
        }

        fseek(fp, 0L, SEEK_END);
        size = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        char buf[size + 1];
        memset(buf, 0, sizeof(buf));
        fread(buf, 1, size, fp);

		MD5Init(&context);
		MD5Update(&context, (const unsigned char *)buf, strlen(buf));
		MD5Final(checksum, &context);
		for (i = 0; i < 16; i++)
		{
			printf ("%02x", (unsigned int) checksum[i]);
		}
		printf(" %s\n", basename(argv[j]));
        fclose(fp);
	}
	return 0;
}

