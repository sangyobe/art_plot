#include "ArbitraryStateDataHandler.h"
#include "mainwindow.h"
#include "pconstants.h"
#include "plotwindow.h"
#include "cmdopts.hpp"
#include "stringutil.hpp"
#include <QApplication>
#include <QDebug>
#include <dtCore/src/dtLog/dtLog.h>


// #define PRINT_PUB_SUB_INFO


ArbitraryStateDataHandler::ArbitraryStateDataHandler(MainWindow* plotToolbox) 
: DataHandler(plotToolbox)
, _plot(std::make_unique<PlotWindow>(plotToolbox))
{
    LOG(info) << "ArbitraryStateDataHandler created.";

    BuildPlots();
}

ArbitraryStateDataHandler::~ArbitraryStateDataHandler()
{
#ifdef USE_TRANSPORT_GRPC
    _sub_reconnector_running = false;
    if (_sub_reconnector.joinable())
        _sub_reconnector.join();
#endif
}

void ArbitraryStateDataHandler::BuildPlots()
{
    //_plot = std::make_unique<PlotWindow>(_plotToolbox);
    _plot->SetWindowTitle("Arbitrary Double Array");
    for (int gi = 0; gi < _data_size_max; gi++) {
        _plot->AddGraph(QString("Data_%1").arg(gi, 2, 10, QLatin1Char('0')), LineColor(gi));
    }
    _plot->show();
    RegisterPlot(_plot.get());


    // 데이터 연결
#ifdef USE_TRANSPORT_ECAL
    _sub_state = std::make_unique<eCAL::protobuf::CSubscriber<dtproto::robot_msgs::ArbitraryStateTimeStamped>>("RobotState");
    _sub_state->AddReceiveCallback(std::bind(
        &ArbitraryStateDataHandler::OnRecvArbitraryStateTimeStamped, this,
        std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4));
#endif
#ifdef USE_TRANSPORT_GRPC
    std::string ip;
    uint16_t port;
    GetServerAddress(ip, port);
    if (ip.empty() || port == 0) {
        ip = "127.0.0.1";
        port = 50051;
    }
    std::string svr_address = string_format("%s:%d", ip.c_str(), port+1);
    
    _sub_state = std::make_unique<dtCore::dtStateSubscriberGrpc<dtproto::robot_msgs::ArbitraryStateTimeStamped>>("RobotState", svr_address);
    std::function<void(dtproto::robot_msgs::ArbitraryStateTimeStamped&)> handler = [this](dtproto::robot_msgs::ArbitraryStateTimeStamped& msg) {
        static long long seq = 0;
        this->OnRecvArbitraryStateTimeStamped("", msg, 0, seq++);
    };
    _sub_state->RegMessageHandler(handler);

    _sub_reconnector_running = true;
    _sub_reconnector = std::thread([this] {

        while (this->_sub_reconnector_running) {
            if (!this->_sub_state->IsRunning()) {
                LOG(warn) << "Disconnected. Reconnecting to ArbitraryState data server...";
                this->_sub_state->Reconnect();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    });
#endif

}


void ArbitraryStateDataHandler::OnRecvArbitraryStateTimeStamped(const char *topic_name,
                                      const dtproto::robot_msgs::ArbitraryStateTimeStamped &state,
                                      const long long time, const long long clock)
{
    double curTime = clock * 1e-3;
    OnRecvArbitraryState(curTime, state.state());

#ifdef PRINT_PUB_SUB_INFO
    qDebug() << "------------------------------------------";
    qDebug() << " Arbitrary State ";
    qDebug() << "------------------------------------------";
    qDebug() << "topic name   : " << topic_name;
    qDebug() << "topic time   : " << time;
    qDebug() << "topic clock  : " << clock;
    qDebug() << "------------------------------------------";
    qDebug() << " Header ";
    qDebug() << "------------------------------------------";
    qDebug() << "seq          : " << state.header().seq();
    qDebug() << "timestamp(s) : " << curTime;
    qDebug() << "------------------------------------------";
    qDebug() << "";
#else
    Q_UNUSED(topic_name);
    Q_UNUSED(state);
    Q_UNUSED(time);
    Q_UNUSED(clock);
#endif // PRINT_PUB_SUB_INFO

}                            

void ArbitraryStateDataHandler::OnRecvArbitraryState(const double curTime, const dtproto::std_msgs::PackedDouble &state)
{
    if (state.GetTypeName() != "dtproto.std_msgs.PackedDouble") {
        return;
    }

    if (_data_size < 0) {
        _data_size = state.data_size();

        // for (int gi = 0; gi < _data_size; gi++) {
        //     _plot->AddGraph(QString("Data_%1.abs").arg(gi, 2, 10, QLatin1Char('0')), LineColor(gi));
        // }
    }

    for (int gi = 0; gi < _data_size; gi++) {
        if (gi >= _data_size_max || gi >= _plot->GetGraphCount())
            return;

        // LOG(trace) << "  data[" << gi << "] = " << state.data(gi);
        _plot->AddData(gi, curTime, state.data().Get(gi));
        _plot->DataUpdated(curTime);
    }
}
