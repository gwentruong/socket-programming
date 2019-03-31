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
    char  line[MAX_LEN] = { '\0' };
    char  s[INET6_ADDRSTRLEN];
    int   n;

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
        perror("Can't send the file");
        return -1;
    }
    printf("Filename sent!\n");

    if (fp == NULL)                     // Open file to read
    {
        perror("Can't open file");
        return -1;
    }
    printf("File can be opened\n");

    // Read the file content to line buf and send the line buf
    while ((n = fread(line, sizeof(char), MAX_LEN, fp)) > 0)
    {
        if (ferror(fp))
        {
            perror("Read file error");
            return -1;
        }

        if (send(sockfd, line, n, 0) == -1)
        {
            perror("Can't send file\n");
            return -1;
        }
        memset(line, 0, MAX_LEN);
    }

    fclose(fp);
    close(sockfd);
    return 0;
}
