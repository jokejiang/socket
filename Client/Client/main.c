//
//  main.c
//  Client
//
//  Created by wangping on 14-2-27.
//  Copyright (c) 2014年 wangping. All rights reserved.
//


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

//端口
#define SERVPORT 1234
//数据缓冲区大小
#define MAX_DATA_SIZE 1024

int main(int argc, char *argv[]) {
    //socket描述符
    int sockfd;
    //发送和接收得字符长度
    long sendBytes,recvBytes;
    //发送和接收数据传送的缓冲区
    char sendBuf[MAX_DATA_SIZE],recvBuf[MAX_DATA_SIZE];
    //服务器端网络地址结构体
    struct sockaddr_in servAddr;
    

    //创建客户端套接字(参数：IPv4协议，面向连接通信，TCP协议)
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("创建套接字失败");
        //退出程序
        exit(1);
    }
    
    //清空网络地址结构体这块内存
    memset(&servAddr,0,sizeof(servAddr));
    //初始化地址结构体
    servAddr.sin_family = AF_INET;//IPv4协议
    servAddr.sin_port = htons(SERVPORT);//端口
    servAddr.sin_addr.s_addr=inet_addr("127.0.0.1");//服务器IP地址
    

    //将套接字绑定到服务器的网络地址上
    if(connect(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr_in)) == -1) {
        perror("fail to connect the socket");
        exit(1);
    }
    printf("Success to connect the socket...\n");
    //将数据传送的缓冲区做清零处理
    memset(recvBuf, 0x00, MAX_DATA_SIZE);
    memset(sendBuf, 0x00, MAX_DATA_SIZE);
    
    while(1) {
        //终端显示客户发送的信息
        printf("Client:");
        //等待用户输入数据
        scanf("%s", sendBuf);
        //想服务端发送数据（参数：套接字、存放要发送数据的缓冲区、缓冲区长度、函数的调用方式）
        if((sendBytes = send(sockfd, sendBuf, strlen(sendBuf), 0)) != strlen(sendBuf)) {
            perror("fail to send datas");
            exit(1);
        }
        //发送成功后将发送数据的缓冲区清零处理
        memset(sendBuf, 0x00, MAX_DATA_SIZE);
        
        //从服务端接收数据信息(参数：套接字、接收数据的缓冲区、缓冲区长度、函数的调用方式)
        if((recvBytes = recv(sockfd, recvBuf, MAX_DATA_SIZE, 0)) == -1) {
            perror("fail to receive datas");
            exit(1);
        }
        //终端显示服务端发来的信息
        printf("Server: %s\n", recvBuf);
        //接收成功后将接收数据的缓冲区清零处理
        memset(recvBuf, 0x00, MAX_DATA_SIZE);
    }
    //关闭套接字
    close(sockfd);
}

