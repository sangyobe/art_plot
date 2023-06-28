#include "recvthreadecal.h"
#include <QDebug>

#define PRINT_PUB_SUB_INFO

void OnControlState_(const char *topic_name_,
                    const art_protocol::quadip::ControlStateTimeStamped &state_,
                    const long long time_, const long long clock_) {
#ifdef PRINT_PUB_SUB_INFO
    qDebug() << "------------------------------------------";
    qDebug() << " QuadIP Control State "                    ;
    qDebug() << "------------------------------------------";
    qDebug() << "topic name   : " << topic_name_            ;
    qDebug() << "topic time   : " << time_                  ;
    qDebug() << "topic clock  : " << clock_                 ;
    qDebug() << "------------------------------------------";
    qDebug() << " Header "                                  ;
    qDebug() << "------------------------------------------";
    qDebug() << "  seq        : " << state_.header().seq()  ;
    qDebug() << "------------------------------------------";
    qDebug() << ""                                          ;
#else
    Q_UNUSED(topic_name_);
    Q_UNUSED(state_);
    Q_UNUSED(time_);
    Q_UNUSED(clock_);
#endif // PRINT_PUB_SUB_INFO
}

RecvThreadECal::RecvThreadECal(QObject *parent)
    : RecvThread(parent)
    , sub_control_state(nullptr)
{
    //connect(this, SIGNAL(), this->parent(), SLOT());
    //connect(this->parent(), SIGNAL(), this, SLOT());

    //connect(this, SIGNAL(new_data_arrived(double*, uint32_t)), this->parent(), SLOT(OnNewDataArrived(double*, uint32_t)));
    //connect(this, &new_data_arrived, this->parent(), &PlotWindow::OnNewDataArrived);
    //connect(_minimap, &Minimap::minimap_scrolled, this, &MapView::on_minimap_scrolled);

    Initialize();
}

RecvThreadECal::~RecvThreadECal()
{
    eCAL::Finalize();
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
    eCAL::Initialize(0, nullptr, "art_plot::QuadIP::ControlStatus");
    eCAL::Process::SetState(proc_sev_healthy, proc_sev_level1, "proc info");
    sub_control_state = new eCAL::protobuf::CSubscriber<art_protocol::quadip::ControlStateTimeStamped>("ControlState");
    auto callback_control_state = std::bind(OnControlState_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    sub_control_state->AddReceiveCallback(callback_control_state);
}

/*!
 * \brief RecvThreadECal::OnRecv
 * 데이터 수신 콜백
 */
void RecvThreadECal::OnRecv(const art_protocol::quadip::ControlStateTimeStamped& data)
{
    Q_UNUSED(data);
    qDebug() << "RecvThreadECal::OnRecv()";
    emit new_data_arrived(0, 0);

}

void RecvThreadECal::OnRefreshPlot()
{
    qDebug() << "RecvThreadECal::OnRefreshPlot";
//    emit add_data_refresh_plot(_dataBuffer, _dataIndex);
//    _dataIndex = 0;
}

void RecvThreadECal::run()
{

    _refreshPlotTimer = new QTimer(this);
    connect(_refreshPlotTimer, SIGNAL(timeout()), this, SLOT(OnRefreshPlot()));
    _refreshPlotTimer->setInterval(1000);

    _refreshPlotTimer->start();
    exec();

    _refreshPlotTimer->stop();

}


