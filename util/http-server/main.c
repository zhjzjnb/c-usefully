

#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>
#include "threadpool.h"

#define SERVER_VERSION "0.0.0"

#define SERVER_STRING "Server: simple/"SERVER_VERSION"\r\n"

#define CONTENT_BODY "<html><script type=\"text/javascript\" src=\"/nav.js\"></script><h1>hello server<h1></html>"

#define GEN_ERR_CODE(code, msg) void response_##code(int fd){\
const char* str = "HTTP/1.1 "#code" BAD REQUEST\r\n"SERVER_STRING"Content-Type: text/html\r\n" "\r\n";\
send(fd,str,strlen(str),0);\
}

GEN_ERR_CODE(400, "BAD REQUEST")

GEN_ERR_CODE(404, "Not Found")


int get_line(int sock, char *buf, int len) {
    int rl = 0;
    int lr = 0, ln = 0;
    char c;
    while (1) {
        int r = recv(sock, &c, 1, 0);
        if (r <= 0) {
            return -1;
        }
        if (c == '\r') {
            if (lr) {
                return -1;
            }
            lr = 1;
            continue;
        } else if (c == '\n') {
            break;
        }

        buf[rl++] = c;
        if (rl == len) {
            return rl;
        }
    }
    return rl;
}

char *get_file_data(const char *fname, size_t *len) {
    FILE *f = fopen(fname, "rb");
    if (f == NULL) {
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    size_t l = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *data = malloc(l);
    fread(data, l, 1, f);
    fclose(f);
    if (len) {
        *len = l;
    }
    return data;
}


void task_func(void *arg) {
    printf("socket work:%u %p\n", pthread_self(), arg);
    int c = (int) arg;
    char buf[1024];

    int rn = get_line(c, buf, sizeof(buf));
    if (!rn) {
        goto LAST;
    }
    printf("head:%s\n", buf);
    char *method = NULL, *url = NULL, *protocol = NULL;
    int head = 0;
    for (int i = 0; i < rn; ++i) {
        if (buf[i] == ' ') {
            buf[i] = '\0';
            if (!method) {
                method = &buf[head];
            } else if (!url) {
                url = &buf[head];
                protocol = &buf[i + 1];
                break;
            }
            head = i + 1;
        }
    }


    if (!method || !url || !protocol) {
        response_400(c);
        goto LAST;
    }
    printf("method:%s url:%s protocol:%s\n", method, url, protocol);

    if (strcasecmp(method, "GET") == 0) {
        url++;
        if (*url == '\0') {
            url = "index.html";
        }

        size_t l;
        char *data = get_file_data(url,&l);
        if(data==NULL){
            response_404(c);
            goto LAST;
        }

        buf[0] = 0;
        int sl = sprintf(buf, "HTTP/1.1 200 OK\r\n");
        sl += sprintf(buf + sl, SERVER_STRING);
        sl += sprintf(buf + sl, "Content-Type: text/html;charset=utf-8\r\n");
        sl += sprintf(buf + sl, "Content-Length:%d\r\n\r\n", l);
        send(c, buf, sl, 0);
        send(c, data, l, 0);
        free(data);
        goto LAST;

    } else if (strcasecmp(method, "POST") == 0) {

    }


    while (1) {
//        int rn = get_line(c, buf, sizeof(buf));
//        if (rn < 0) {
//            break;
//        }
//        printf("get line:%s\n",buf);
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


//        printf("%s\n%d %d\n", buf, ns, sl);


//
        break;
    }
    LAST:

    close(c);

}

/*
 * 传递端口号  0的话 动态分配
 */
int startup(uint16_t *port) {
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
    if (bind(httpd, (struct sockaddr *) &name, sizeof(name)) < 0)
        err_exit("bind");
    if (*port == 0)  /* if dynamically allocating a port */
    {
        socklen_t namelen = sizeof(name);
        if (getsockname(httpd, (struct sockaddr *) &name, &namelen) == -1)
            err_exit("getsockname");
        *port = ntohs(name.sin_port);
    }
    if (listen(httpd, SOMAXCONN) < 0)
        err_exit("listen");
    return (httpd);
}

int main(int argc, char **argv) {
    threadpool_t pool;
    threadpool_init(&pool, 5, 128);


    uint16_t port = 8000;
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
        client_sock = accept(server_sock, (struct sockaddr *) &client_name, &client_name_len);
        if (client_sock == -1)
            err_exit("accept");
        thread_task t = {task_func, (void *) client_sock};
        threadpool_submit(&pool, t);
    }
    close(server_sock);
    threadpool_destroy(&pool);
    return 0;
}