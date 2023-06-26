#ifndef RECVTHREAD_H
#define RECVTHREAD_H

#include <QThread>

/*!
 * \brief 원격에서 데이터를 수신하여 그래프를 업데이트하는 쓰레드의 베이스 클래스
 * \param parent
 */
class RecvThread : public QThread
{
    Q_OBJECT
public:
    explicit RecvThread(QObject *parent = nullptr);
    virtual ~RecvThread();

    void run();

public:
    virtual void Initialize() = 0;

signals:

public slots:
};

#endif // RECVTHREAD_H
