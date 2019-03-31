#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "md5.h"

int main (int argc, char **argv)
{
	struct MD5Context context;
	unsigned char checksum[16];
	int i;
	int j;

	if (argc < 2)
	{
		fprintf (stderr, "usage: %s string-to-hash\n", argv[0]);
		exit (1);
	}
	for (j = 1; j < argc; ++j)
	{
		printf("MD5 (\"%s\") = ", argv[j]);
		MD5Init(&context);
		MD5Update(&context, (const unsigned char *)argv[j], strlen(argv[j]));
		MD5Final(checksum, &context);
		for (i = 0; i < 16; i++)
		{
			printf ("%02x", (unsigned int) checksum[i]);
		}
		printf ("\n");
	}
	return 0;
}
