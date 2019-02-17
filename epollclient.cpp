#include <arpa/inet.h>
#include <iostream>


#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <unistd.h>


#define PORT 888888

int main(int argc, char *argv[], char *evp[])
{
    int clientfd;
    int on = 1;
    char buffer[512] = {"hello"};

    struct sockaddr_in serverAddr, clientAddr;
    
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_port = htons(1234567);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_port = htons((short)(PORT));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if ((clientfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        std::cout << "create client error!" << std::endl;
        return -1;
    }

    if (connect(clientfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cout << "connect server error!" << std::endl;
        return -1;
    }

    if (write(clientfd, buffer, sizeof(buffer)) < 0)
    {
        std::cout << "write message error!" << std::endl;
        return -1;
    }
    read(clientfd, buffer, sizeof(buffer));
    exit(0);
}
