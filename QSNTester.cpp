#include "QSNTester.h"
#include <unistd.h>
#include <errno.h>

QSNTester::QSNTester(int fd, QObject *p) : QObject(p) {
	qWarning("QSNTester: creating socket notifier...");
	qsn = new QSocketNotifier(fd, QSocketNotifier::Read, this);
	connect(qsn, SIGNAL(activated(int)), this, SLOT(readyRead(int)), Qt::QueuedConnection);
}

void QSNTester::readyRead(int fd) {
	qWarning("readyRead invoked!");

	char c = 0;
	int err;
retry:
	err  = read(fd, &c, sizeof(c));
	if (err == -1) {
		if (errno == EINTR) {
			goto retry;
		}
		qWarning("read: %s", strerror(errno));
		exit(1);
	}

	qWarning("readyRead read '%c'", c);
}
