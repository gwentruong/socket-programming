#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <libgen.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "md5sum/md5.h"

#define PORT "3490"
#define MAX_LEN 4096

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        perror("Usage: send_file <IP address> <file/path>");
        return -1;
    }

    struct addrinfo hints, *servinfo, *p;
    int  sockfd;
    int  rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // Loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    // Connected to server, processing the file
    FILE *fp            = fopen(argv[2], "r");
    char *filename      = basename(argv[2]);
    char  buf[MAX_LEN]  = { '\0' };
    char  s[INET6_ADDRSTRLEN];
    char  checksum[33] = { '\0' };
    int   n, size;

    // Print the server IP address in readable form
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);
    freeaddrinfo(servinfo);

    if (filename == NULL)               // Find file's path
    {
        perror("Can't find file path.");
        return -1;
    }
    printf("Found the file path\n");

    strcpy(buf, filename);              // Copy filename to buf

    if (send(sockfd, buf, MAX_LEN, 0) == -1)    // Send filename
    {
        perror("Can't send the filename");
        return -1;
    }
    printf("Filename sent!\n");
    memset(buf, 0, MAX_LEN);

    char *check = md5checksum(argv[2]);
    if (check == NULL)
    {
        perror("Can't open file");
        return -1;
    }
    else
        strcpy(checksum, check);
    printf("File can be opened\n");

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    char file_content[size + 1];
    memset(file_content, 0, sizeof(file_content));
    snprintf(buf, MAX_LEN, "%d", size);     // Assign the size of file to buf
    printf("Size of file %s\n", buf);

    if (send(sockfd, buf, MAX_LEN, 0) == -1)    // Send file size
    {
        perror("Can't send the file size");
        return -1;
    }
    printf("File size sent!\n");

    // Read the file content to file_content buf and send the file_content buf
    if ((n = fread(file_content, sizeof(char), MAX_LEN, fp)) > 0)
    {
        if (ferror(fp))
        {
            perror("Read file error");
            return -1;
        }

        if (send(sockfd, file_content, n, 0) == -1)
        {
            perror("Can't send file\n");
            return -1;
        }

        if (send(sockfd, checksum, 33, 0) == -1)
        {
            perror("Can't send checksum\n");
            return -1;
        }
        memset(file_content, 0, sizeof(file_content));
        memset(checksum, 0, sizeof(checksum));
    }

    fclose(fp);
    close(sockfd);
    return 0;
}
