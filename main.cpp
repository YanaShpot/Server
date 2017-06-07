/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctime>

#define SIZE 3

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int WriteDateTime(bool isTime, int newsockfd) {
    int isWritten;
    time_t rawtime;
    struct tm * timeinfo;
    char buff[11]; //пам'ять не виділиться, якщо функція не запуститься. А функція за програму і так запуститься максимум раз.

    time (&rawtime);
    timeinfo = localtime(&rawtime);
    if (isTime) {
        //strftime(buff,9,"%H:%M:%S",timeinfo);
        strftime(buff, sizeof(buff),"%H:%M:%S",timeinfo);
    }
    else {
        strftime(buff,sizeof(buff),"%d-%m-%Y",timeinfo);
        //strftime(buff,11,"%d-%m-%Y",timeinfo);
    }

    isWritten = write(newsockfd, buff, strlen(buff));

    delete[] timeinfo;

    return isWritten;

}

void WriteMessage(uint16_t len, int newsockfd) {
    char buff[len];
    int n = read(newsockfd, buff, len);
    if (n < 0)
    {
        error("ERROR reading from socket");
    }

    n = write(newsockfd, buff, len );
    if (n < 0)
    {
        error("ERROR writing to socket");
    }

}

int main()
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 12347;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);


    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd,5);  //даємо слухати
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,
                       (struct sockaddr *) &cli_addr,
                       &clilen); //повертає дескриптор конекшн.

        if (newsockfd < 0)
        {
            error("ERROR on accept");
        }

        bzero(buffer, SIZE);
        n = read(newsockfd, buffer, 1);

        if (n < 0)
        {
            error("ERROR reading from socket");
        }
        printf("Client: %s\n", buffer);

        if (buffer[0] == 't')
        {

            n = WriteDateTime(true,newsockfd);
            if (n < 0)
            {
                error("ERROR writing to socket");
            }


        }
        else if (buffer[0] == 'd')
        {

            n = WriteDateTime(false, newsockfd);
            if (n < 0)
            {
                error("ERROR writing to socket");
            }

        }
        else if (buffer[0] == 'h') {
            strcpy(buffer,"Well hello there!");
            n = write(newsockfd, buffer, strlen(buffer));
            if (n < 0)
            {
                error("ERROR writing to socket");
            }

        }
        else if (buffer[0] == 'm')
        {
            n = read(newsockfd, buffer + 1, 2);
            if (n < 0)
            {
                error("ERROR reading from socket");
            }
            uint16_t len;
            memcpy(&len, buffer + 1, sizeof(uint16_t));

            WriteMessage(len, newsockfd);


        }


    close(newsockfd);
    close(sockfd);

    return 0;
}
