

#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>
#include "threadpool.h"

#define SERVER_VERSION "0.0.0"

#define SERVER_STRING "Server: simple/"SERVER_VERSION"\r\n"

#define CONTENT_BODY "<html><h1>hello server<h1></html>"

void task_func(void* arg) {
    printf("socket work:%u %p\n", pthread_self(), arg);
    int c = (int) arg;
    char buf[1024];
    int rn = 0;

    while (1) {
        int r = recv(c, buf, sizeof(buf), 0);

        if (r <= 0) {
//            printf("recv:%d\n", r);
//            printf("errno值： %d\n", errno);
//            printf("错误信息： %s\n", strerror(errno));
            return;
        }
        printf("recv from:%d\n%s\n", r, buf);

        buf[0] = 0;
        int sl = sprintf(buf, "HTTP/1.1 200 OK\r\n");
        sl += sprintf(buf + sl, SERVER_STRING);
//        sprintf(buf,"Content-Type: application/json\r\n\r\n");
        sl += sprintf(buf + sl, "Content-Type: text/html;charset=utf-8\r\n");
        sl += sprintf(buf + sl, "Content-Length:%d\r\n\r\n", strlen(CONTENT_BODY));

        sl += sprintf(buf + sl, CONTENT_BODY);
        int ns = send(c, buf, sl, 0);


        printf("%s\n%d %d\n",buf, ns,sl);


        close(c);
        break;
    }


}

/*
 * 传递端口号  0的话 动态分配
 */
int startup(uint16_t* port) {
    int httpd = 0;
    int on = 1;
    struct sockaddr_in name;

    httpd = socket(AF_INET, SOCK_STREAM, 0);
    if (httpd == -1)
        err_exit("socket");
    memset(&name, 0, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_port = htons(*port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    if ((setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) {
        err_exit("setsockopt failed");
    }
    if (bind(httpd, (struct sockaddr*) &name, sizeof(name)) < 0)
        err_exit("bind");
    if (*port == 0)  /* if dynamically allocating a port */
    {
        socklen_t namelen = sizeof(name);
        if (getsockname(httpd, (struct sockaddr*) &name, &namelen) == -1)
            err_exit("getsockname");
        *port = ntohs(name.sin_port);
    }
    if (listen(httpd, SOMAXCONN) < 0)
        err_exit("listen");
    return (httpd);
}

int main(int argc, char** argv) {
    threadpool_t pool;
    threadpool_init(&pool, 5, 128);


    uint16_t port = 8080;
    if (argc == 2) {
        port = atoi(argv[1]);
    }
    if (port < 0) {
        err_exit("your port is not valid");
    }
    int server_sock = startup(&port);

    int client_sock = -1;
    struct sockaddr_in client_name;
    socklen_t client_name_len = sizeof(client_name);

    printf("httpd running on port %d\n", port);


    while (1) {
        client_sock = accept(server_sock, (struct sockaddr*) &client_name, &client_name_len);
        if (client_sock == -1)
            err_exit("accept");
        thread_task t = {task_func, (void*) client_sock};
        threadpool_submit(&pool, t);
    }
    close(server_sock);
    threadpool_destroy(&pool);
    return 0;
}