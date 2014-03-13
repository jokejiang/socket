//
//  main.c
//  Server
//
//  Created by wangping on 14-2-27.
//  Copyright (c) 2014年 wangping. All rights reserved.


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

//监听端口
#define SERVPORT 1234
//最大连接数
#define BACKLOG 20
//文件描述符范围，linux下不能为0
#define MAX_CON_NO 10
#define MAX_DATA_SIZE 1024  //数据缓冲区大小

int main(int argc, char *argv[]) {
    //服务端（告诉socket应该再什么地方监听）和客户端 网络地址信息
    struct sockaddr_in serverSockaddr, clientSockaddr;
    //地址信息大小
    unsigned int sinSize;
    //接收和发送数据的长度
    long recvBytes, sendBytes;
    //表示一组等待检查的套接口
    fd_set readfd;
    //服务端和客户端的套接字
    int sockfd, clientfd;
    //发送和接收数据的缓冲区
    char sendBuf[MAX_DATA_SIZE], recvBuf[MAX_DATA_SIZE];
    
    //创建服务端套接字
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("fail to establish a socket");
        exit(1);
    }
    //显示服务端套接字文件描述符
    printf("Success to establish a socket...(sockfd = %d)\n", sockfd);
    
    //清空网络地址结构体这块内存
    memset(&serverSockaddr,0,sizeof(serverSockaddr));
    serverSockaddr.sin_family = AF_INET;//IPv4协议
    serverSockaddr.sin_port = htons(SERVPORT);//端口
    serverSockaddr.sin_addr.s_addr = htonl(INADDR_ANY);//在本机的所有ip上开始监听
    
    int on = 1;
    //设置套接字(参数：套接字、基本套接口、允许在bind ()过程中本地地址可重复使用、一般不会立即关闭而经历TIME_WAIT的过程）后想继续重用该socket)
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    
    //把本机地址信息赋给socket
    if(bind(sockfd, (struct sockaddr *)&serverSockaddr, sizeof(struct sockaddr))== -1) {
        perror("fail to bind");
        exit(1);
    }
    printf("Success to bind the socket...\n");
    
    
    //监听这个socket，最大支持BACKLOG个链接，如果客户端调用connect()发出连接请求，服务器端就会接收到这个请求
    if(listen(sockfd, BACKLOG) == -1) {
        perror("fail to listen");
        exit(1);
    }
    printf("Success to listen on the socket...\n");
    
    while(1) {
        //将readfd初始化为空集NULL
        FD_ZERO(&readfd);
        //把sockfd放入要测试的描述符集，这样下一步调用select对readfd进行测试的时候就会测试sockfd了(因为我们将sockfd放入的readfd) 一个描述符集可以包含多个被测试的描述符,
        FD_SET(sockfd, &readfd);
        sinSize = sizeof(struct sockaddr_in);
        //监视套接字的变化情况-读写或是异常，将一直阻塞到有一个socket满足条件（若将时间值设为0秒0毫秒，就变成一个纯粹的非阻塞函数，不管文件描述符是否有变化，都立刻返回继续执行，文件无变化返回0，有变化返回一个正值）
        if(select(MAX_CON_NO, &readfd, NULL, NULL, (struct timeval *)0) > 0) {
             //有一个描述符sockfd准备好了
            if(FD_ISSET(sockfd, &readfd) > 0) {
                
                //接受一个客户端的请求链接，并保存该客户端的套接字
                if((clientfd = accept(sockfd, (struct sockaddr *)&clientSockaddr, &sinSize)) == -1) {
                    perror("fail to accept");
                    exit(1);
                }
                printf("Success to accpet a connection request...\n");
                printf("%s join in!\n", inet_ntoa(clientSockaddr.sin_addr));//include color set
                
                while(1) {
                    //从客户端接收的数据
                    if((recvBytes = recv(clientfd, recvBuf, MAX_DATA_SIZE, 0)) == -1) {
                        perror("fail to receive datas");
                        exit(1);
                    }
                    printf("Client:%s\n", recvBuf);
                    memset(recvBuf, 0x00, MAX_DATA_SIZE);
                    //给客户端发送数据
                    printf("Server:");
                    scanf("%s", sendBuf);
                    if((sendBytes = send(clientfd, sendBuf, strlen(sendBuf), 0)) != strlen(sendBuf)) {
                        perror("fail to send datas");
                        exit(1);
                    }
                    memset(sendBuf, 0x00, MAX_DATA_SIZE);
                }
            }
            close(sockfd);
        }  
    }  
}
