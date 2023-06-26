#include "recvthread.h"


RecvThread::RecvThread(QObject *parent) : QThread(parent)
{

}

RecvThread::~RecvThread()
{
}

void RecvThread::run()
{
    this->exec();
}
