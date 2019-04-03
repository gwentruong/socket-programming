#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>
#include "md5.h"

int main (int argc, char **argv)
{
	if (argc < 2)
	{
		fprintf (stderr, "Usage: %s <file> [<file> [...] ]\n", argv[0]);
		exit(1);
	}

	for (int j = 1; j < argc; ++j)
	{
        printf("%s", md5checksum(argv[j]));
		// MD5Init(&context);
		// MD5Update(&context, (const unsigned char *)buf, strlen(buf));
		// MD5Final(checksum, &context);
		// for (i = 0; i < 16; i++)
		// {
		// 	printf ("%02x", (unsigned int) checksum[i]);
		// }
		printf(" %s\n", basename(argv[j]));
	}
	return 0;
}
