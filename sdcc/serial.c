#include <stdio.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define CHECK_RET(tag, flag) if(flag){printf(#tag" %d %s\n", errno, strerror(errno));exit(-1);}

void set_baudrate(struct termios *opt, unsigned int baudrate) {
    cfsetispeed(opt, baudrate);
    cfsetospeed(opt, baudrate);
}

static void set_data_bit(struct termios *opt, unsigned int databit) {
    opt->c_cflag &= ~CSIZE;
    switch (databit) {
        case 8:
            opt->c_cflag |= CS8;
            break;
        case 7:
            opt->c_cflag |= CS7;
            break;
        case 6:
            opt->c_cflag |= CS6;
            break;
        case 5:
            opt->c_cflag |= CS5;
            break;
        default:
            opt->c_cflag |= CS8;
            break;
    }
}

static void set_parity(struct termios *opt, char parity) {
    switch (parity) {
        case 'N':                  /* no parity check */
            opt->c_cflag &= ~PARENB;
            break;
        case 'E':                  /* even */
            opt->c_cflag |= PARENB;
            opt->c_cflag &= ~PARODD;
            break;
        case 'O':                  /* odd */
            opt->c_cflag |= PARENB;
            opt->c_cflag |= ~PARODD;
            break;
        default:                   /* no parity check */
            opt->c_cflag &= ~PARENB;
            break;
    }
}

static void set_stop_bit(struct termios *opt, int stopbit) {
    if (stopbit == 1) {
        opt->c_cflag &= ~CSTOPB; /* 1 stop bit */
    } else if (stopbit == 2) {
        opt->c_cflag |= CSTOPB;  /* 2 stop bits */
    }
}

/**
 * @brief set_port_attr  com口属性设置，
 * @param fd
 * @param baudrate
 * @param databit    // 5, 6, 7, 8
 * @param stopbit //  "1", "1.5", "2"
 * @param parity // N(o), O(dd), E(ven)
 * @param vtime
 * @param vmin
 * @return
 */
int set_port_attr(int fd, int baudrate, int databit, int stopbit, char parity, int vtime, int vmin) {
    struct termios opt;
    tcgetattr(fd, &opt);
    //设置波特率
    set_baudrate(&opt, baudrate);
    opt.c_cflag |= CLOCAL | CREAD;      /* | CRTSCTS */


    //设置数据位
    set_data_bit(&opt, databit);
    //设置校验位
    set_parity(&opt, parity);
    //设置停止位
    set_stop_bit(&opt, stopbit);
    //其它设置
    opt.c_oflag = 0;
    opt.c_lflag |= 0;
    opt.c_oflag &= ~OPOST;
    opt.c_cc[VTIME] = vtime;
    opt.c_cc[VMIN] = vmin;
    tcflush(fd, TCIFLUSH);
    return (tcsetattr(fd, TCSANOW, &opt));
}


int read_serial(int fd, char *buff, int len) {
    fd_set rfds;
    int retval;
    struct timeval tv;//100000us = 0.1s
//    tv.tv_sec = 0;
//    tv.tv_usec = 30*1000;
    int ret, pos=0;
    while (1) {
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        retval = select(fd + 1, &rfds, NULL, NULL, &tv);

        if (retval == -1) {
            //perror("select check is it readable error!");
            break;
        } else if (retval) {
            ret = read(fd, buff + pos, 1);
            if (-1 == ret) {
                break;
            }
            pos++;
            if (len <= pos) {
                break;
            }
        } else {
            break;
        }
    }

    return pos;
}

int main(void) {
    int fd = open("/dev/tty.wchusbserial14330", O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd == -1) {
        printf("can not open the file\n");
        return 1;
    }

    int ret = set_port_attr(fd, 9600, 8, 1, '1', 0, 0);
    CHECK_RET(set_port_attr, ret != 0)


    printf("successful to open the file\n");
    ret = write(fd, "abc", 3);
    CHECK_RET(write, ret <= 0)


    char buff[32] = {0};
    read_serial(fd, buff, 32);


    printf("recv is :%s\n", buff);


    close(fd);

    return 0;

}
