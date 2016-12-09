#ifndef _LIBEPOLL_H_
#define _LIBEPOLL_H_

#include <sys/time.h>
#include <sys/epoll.h>

#include <libubox/uloop.h>
#include <libubox/utils.h>

#include "utils.h"

struct epoll_handler_s {
    struct list_head list;
    int fd;
    void (*cb)(struct epoll_handler_s *h,uint32_t events);
};

struct epoll_timeout_s {
    struct list_head list;
    Bool pending;
    void (*cb)(struct epoll_timeout_s *t);
    struct timeval time;
};


int timer_del(struct epoll_timeout_s *timer);
int timer_add(struct epoll_timeout_s *timer, int msecs);
int epoll_init(void);
int epoll_add(struct epoll_handler_s *h,uint32_t events);
void epoll_loop(void);
int epoll_del(struct epoll_handler_s *h);
#endif
