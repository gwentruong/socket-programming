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

#define PORT "8080"
#define MAX_LEN 1024

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        perror("Usage: send_file <IP address>");
        return -1;
    }

    struct addrinfo hints, *servinfo, *p;
    int   sockfd;
    int   rv;
    char  s[INET6_ADDRSTRLEN];
    char *request = "GET /index.html HTTP/1.1\r\n\r\n";
    char  buf[MAX_LEN] = { '\0' };
    int   n;

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

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);
    freeaddrinfo(servinfo);

    if (send(sockfd, request, strlen(request), 0) == -1)
    {
        perror("Can't send request to server");
        return -1;
    }
    printf("Request to server sent!\n");


    while ((n = recv(sockfd, buf, MAX_LEN, 0)) > 0)
    {
        printf("%s\n", buf);
    }

    printf("File is fetched successfully\n");

    close(sockfd);
    return 0;
}
