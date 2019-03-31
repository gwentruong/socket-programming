#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "3490"
#define MAX_LEN 4096
#define BACKLOG 10

void sigchld_handler(int s)
{
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main (void)
{
    int sockfd, new_fd;
    struct addrinfo hints, *servinfo, *p;
    struct sigaction sa;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    int yes = 1;
    int rv;
    char s[INET6_ADDRSTRLEN];
    char filename[MAX_LEN] = { '\0' };

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        // Return file descriptor
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        // Set socket option
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        // Try to bind socket with IP address
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    // Nothing binded with p, failed
    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    // Fail to listen
    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    inet_ntop(their_addr.ss_family,
              get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
    printf("server: got connection from %s\n", s);

    while (1)
    {
        sin_size = sizeof(their_addr);
        new_fd   = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }

        if (!fork())
        {
            if (recv(new_fd, filename, MAX_LEN, 0) == -1)
            {
                perror("Can't receive filename");
                return -1;
            }
            printf("Received filename\n");

            FILE *fp = fopen(filename, "wb");
            if (fp == NULL)
            {
                perror("Can't open file to write");
                return -1;
            }

            ssize_t n;
            char buf[MAX_LEN] = { '\0' };
            while((n = recv(new_fd, buf, MAX_LEN, 0)) > 0)
            {
                if (n == -1)
                {
                    perror("Error occurs while receiving file");
                    return -1;
                }
                printf("No error in receiving\n");

                if (fwrite(buf, sizeof(char), n, fp) != n)
                {
                    perror("Writing file error");
                    return -1;
                }
                printf("File transfered successfully\n");
                memset(buf, 0, MAX_LEN);
            }
            fclose(fp);
            close(new_fd);
        }
    }
    
    close(sockfd);
    return 0;
}
