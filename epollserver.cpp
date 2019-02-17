#include <arpa/inet.h>
#include <iostream>

#include <fcntl.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include <signal.h>

#include <string.h>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>

#include <sys/epoll.h>

#include <unistd.h> // write/read


#define MAXFDS 256
#define EVENTS 100
#define PORT 8888

bool setNonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    if (-1 == fcntl(fd, F_SETFL, flags))
        return false;
    return true;
}

int main()
{
    int epfd, fd, nfds, connfd, listenfd;
    int on = 1;
    char* buffer[512];
    struct sockaddr_in serverAddr, clientAddr;
    struct epoll_event ev, events[EVENTS];
 

//    struct sigaction sig;
//    sigemptyset(&sig.sa_mask);
//    sig_handler = SIG_IGN;
//    sigaction(SIGPIPE, &N > sig, NULL);

    epfd = epoll_create(MAXFDS);
    
//    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //int socket( int af, int type, int protocol)
    if (-1 == listenfd)
    {
        std::cout << "create server socket error!" << std::endl;
        return -1;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    int port = 1234567;
    serverAddr.sin_port = htons(port);
    std::string ip("127.0.0.1");
    serverAddr.sin_addr.s_addr = inet_addr(ip.data());

    if (0 != bind(listenfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) //int bind( int sockfd , const struct sockaddr * my_addr, socklen_t addrlen)
    {
        std::cout << "server socket bind error!" << std::endl;
        return -1;
    }

    if (-1 == listen(listenfd, 32))
    {
        std::cout << "listen socket error!" << std::endl;
        return -1;
    }

//    ev.data.fd = sfd;
//    ev.events = EPOLLIN;
//    epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &ev);

    while(true)
    {
        nfds = epoll_wait(epfd, events, MAXFDS, 0);//epoll_wait(int epfd, epoll_event* events, int max events, int timeout)
        std::cout << "nfds = " << nfds << std::endl;
        sleep(2);

        for (int i = 0; i < nfds; ++i)
        {
            if (listenfd == events[i].data.fd)
            {
                memset(&clientAddr, 0, sizeof(clientAddr));
                socklen_t len = sizeof(clientAddr);
                connfd = accept(listenfd, (struct sockaddr*)&clientAddr, &len);//accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
                if (-1 == connfd)
                {
                    std::cout << "accept error!" << std::endl;
                    break;
                }

                std::cout << connfd << " connected!" << std::endl;

                setNonBlock(connfd);

                ev.data.fd = connfd;
                ev.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
            }
            else if (events[i].data.fd & EPOLLIN)
            {
                bzero(&buffer, sizeof(buffer));
                std::cout << "server receive message" << std::endl;
                int ret = read(events[i].data.fd, buffer, sizeof(buffer));
                if (ret < 0)
                {
                    std::cout << "receive message error." << std::endl;
                    return -1;
                }

                std::cout << "receive message is : " << (char*)buffer << std::endl;
                ev.data.fd = events[i].data.fd;
                ev.events = EPOLLOUT;
                epoll_ctl(epfd, EPOLL_CTL_MOD, events[i].data.fd, &ev);
            }
            else if (events[i].data.fd & EPOLLOUT)
            {
                bzero(&buffer, sizeof(buffer));
                bcopy("EPOLLOUT!", buffer, sizeof("EPOLLOUT!"));
                int ret = write(events[i].data.fd, buffer, sizeof(buffer));
                if (ret < 0)
                {
                    std::cout << "send message error!" << std::endl;
                    return -1;
                }
                
                ev.data.fd = events[i].data.fd;
                epoll_ctl(epfd, EPOLL_CTL_DEL, ev.data.fd, &ev);
            }
        }
    }

    if (fd > 0)
    {
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }
}
