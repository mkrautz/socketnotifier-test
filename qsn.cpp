#include <QtCore>

#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>

#include "QSNTester.h"

void *thr_func(void *sptr) {
	int tsock = *((int *)sptr); 
	char c = 0;
	while (1) {
		int err = read(tsock, &c, sizeof(c));
		if (err == -1) {
			if (errno == EINTR) {
				continue;
			}
			fprintf(stderr, "read: %s\n", strerror(errno));
			exit(1);
		}

		fprintf(stderr, "thr: read '%c'!\n", c);

retry:
		c = 'a';
		err = write(tsock, &c, sizeof(c));
		if (err == -1) {
			if (errno == EINTR) {
				goto retry;
			}
			fprintf(stderr, "write: %s\n", strerror(errno));
			exit(1);
		}

		fprintf(stderr, "thr: wrote '%c'\n", c);
		fprintf(stderr, "thr: now sleeping...\n");

		while (1) {
			usleep(1000);
		}
	}
	return NULL;
}

int main(int argc, char **argv) {
	pthread_t thr;
	int fd[2] = { -1, -1 };
	int err = socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
	if (err == -1) {
		fprintf(stderr, "socketpair: %s\n", strerror(errno));
		exit(1);
	}

	fprintf(stderr, "main:  tsock=%i,  mainsock=%i\n", fd[0], fd[1]);

	if (pthread_create(&thr, NULL, thr_func, &fd[0]) == -1) {
		fprintf(stderr, "pthread_create: %s\n", strerror(errno));
		exit(1);
	}

	char c = 'x';
	fprintf(stderr, "main: about to write '%c'\n", c);
	err = write(fd[1], &c, sizeof(c));
	if (err == -1) {
		fprintf(stderr, "write: %s\n", strerror(errno));
		exit(1);
	}

	QCoreApplication a(argc, argv);
	QSNTester *qsnt = new QSNTester(fd[1], NULL);
	(void) qsnt;
	
	unsigned long i;
	while (1) {
		qWarning("main: main loop iter %lu entry", i);
		a.processEvents();
		qWarning("main: main loop iter %lu exit", i);
		++i;
	}
}
