#include <QtCore>

class QSNTester : public QObject {
	Q_OBJECT
public:
	QSNTester(int fd, QObject *p = NULL);
private:
	QSocketNotifier *qsn;
private slots:
	void readyRead(int fd);
};
