#include "recvthreadecal.h"
#include <QDebug>



RecvThreadECal::RecvThreadECal(QObject *parent)
    : RecvThread(parent)
{
    //connect(this, SIGNAL(), this->parent(), SLOT());
    //connect(this->parent(), SIGNAL(), this, SLOT());

    //connect(this, SIGNAL(new_data_arrived(double*, uint32_t)), this->parent(), SLOT(OnNewDataArrived(double*, uint32_t)));
    //connect(this, &new_data_arrived, this->parent(), &PlotWindow::OnNewDataArrived);
    //connect(_minimap, &Minimap::minimap_scrolled, this, &MapView::on_minimap_scrolled);

}

/*!
 * \brief RecvThreadECal::Initialize
 *
 * 데이터 수신을 위한 초기화를 진행합니다.
 * 1. eCAL subsystem 초기화
 * 2. 데이터 수신 subscriber 생성
 */
void RecvThreadECal::Initialize()
{

}

/*!
 * \brief RecvThreadECal::OnRecv
 * 데이터 수신 콜백
 */
void RecvThreadECal::OnRecv()
{
    qDebug() << "RecvThreadECal::OnRecv()";
    emit new_data_arrived(0, 0);

}

void RecvThreadECal::run()
{

    threadTimer = new QTimer(this);
    connect(threadTimer, SIGNAL(timeout()), this, SLOT(OnRecv()));
    threadTimer->setInterval(1000);

    threadTimer->start();
    exec();

    threadTimer->stop();

}


