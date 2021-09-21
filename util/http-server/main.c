

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

void get_file_extension(const char *fname, char *out, size_t *l) {
    *l = 0;

    int len = strlen(fname);

    for (int i = len - 1; i >= 0; --i) {
        if (fname[i] == '.') {
            break;
        }
        (*l)++;
    }

    for (int i = 0; i < *l; i++) {
        out[i] = fname[len + i - *l];
    }
    out[*l] = 0;

}

const char *get_content_type(const char *ext) {
#define CASE_TYPE(tt, ee, append) if(!strcasecmp(ext,ee)) return tt"/"ee append

    CASE_TYPE("image", "jpg", "");
    CASE_TYPE("image", "png", "");

    CASE_TYPE("text", "js", ";charset=utf-8");
    CASE_TYPE("text", "html", ";charset=utf-8");

    return "not impl";
}

char *get_file_data(const char *fname, size_t *len) {
    if (*fname == '/') {
        fname++;
    }
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

void skip_recv(int fd) {
    char buf[1024] = {0};
    while (1) {
        int rn = get_line(fd, buf, sizeof(buf));

        if (!rn) {
            break;
        }
        if (buf[0] == '\n') {
            break;
        }
    }
}


void task_func(void *arg) {
    printf("socket work:%u %p\n", pthread_self(), arg);
    int c = (int) arg;
    char buf[1024] = {0};

    int rn = get_line(c, buf, sizeof(buf));
    if (!rn) {
        goto LAST;
    }
    printf("head:%s :%d\n", buf, rn);
    char *method = NULL, *url = NULL, *protocol = NULL;
    int head = 0;
    int l = 0;
    for (int i = 0; i < rn; ++i) {
        if (buf[i] == ' ') {
            buf[i] = '\0';
            if (!method) {
                l = i - head + 1;
                method = malloc(l);
                memcpy(method, &buf[head], l - 1);
                method[l - 1] = 0;
            } else if (!url) {
                l = i - head + 1;
                url = malloc(l);
                memcpy(url, &buf[head], l - 1);
                url[l - 1] = 0;

                protocol = malloc(rn - i);
                memcpy(protocol, &buf[i + 1], rn - i - 1);
                protocol[rn - i - 1] = 0;
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
        skip_recv(c);

        if (*(url + 1) == '\0') {
            url = realloc(url, 11);
            memcpy(url, "index.html", 10);
            url[10] = 0;
        }

        size_t l;
        char *data = get_file_data(url, &l);
        if (data == NULL) {
            response_404(c);
            goto LAST;
        }

        char exc[64] = {0};
        size_t exl;
        get_file_extension(url, exc, &exl);

        const char *cont = get_content_type(exc);

        buf[0] = 0;
        int sl = sprintf(buf, "HTTP/1.1 200 OK\r\n");
        sl += sprintf(buf + sl, SERVER_STRING);
        sl += sprintf(buf + sl, "Content-Type: %s\r\n", cont);
        sl += sprintf(buf + sl, "Content-Length:%d\r\n\r\n", l);
        int ls = send(c, buf, sl, 0);

//        printf("%d  %d\n", ls, sl);
        ls = send(c, data, l, 0);

//        printf("%d  %d\n", ls, l);
        free(data);


        goto LAST;

    } else if (strcasecmp(method, "POST") == 0) {

    }


    LAST:
    free(method);
    free(url);
    free(protocol);
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