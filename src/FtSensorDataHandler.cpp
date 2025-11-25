#include "FtSensorDataHandler.h"
#include "cmdopts.hpp"
#include "mainwindow.h"
#include "pconstants.h"
#include "plotwindow.h"
#include "stringutil.hpp"
#include <QApplication>
#include <QDebug>
#include <dtCore/src/dtLog/dtLog.h>
#include <dtMath/dtMath.h>

#define MCAP_COMPRESSION_NO_LZ4
#define MCAP_COMPRESSION_NO_ZSTD
#include <mcap/mcap.hpp>

#define USE_LOGGINGTIME_AS_TIMESTAMP

// #define PRINT_PUB_SUB_INFO

#define ENABLE_FT_PLOT

static const double RAD2DEG = 57.295779513; //! 180.0f/M_PI
static const double DEG2RAD = 0.0174532925; //! M_PI/180.0f

FtSensorDataHandler::FtSensorDataHandler(MainWindow *plotToolbox)
    : DataHandler(plotToolbox, "Ft")
#ifdef ENABLE_FT_PLOT
      ,
      _plot_ft(std::make_unique<PlotWindow>(plotToolbox))
#endif
{
    LOG(info) << "FtSensorDataHandler created.";

    BuildPlots();
}

FtSensorDataHandler::~FtSensorDataHandler()
{
#ifdef USE_TRANSPORT_GRPC
    _sub_reconnector_running = false;
    if (_sub_reconnector.joinable())
        _sub_reconnector.join();
#endif
}

void FtSensorDataHandler::BuildPlots()
{
#ifdef ENABLE_FT_PLOT
    // _plot_ft = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_ft->SetWindowTitle("Ft");
    _plot_ft->AddGraph("Force.x", LineColor<0>());
    _plot_ft->AddGraph("Force.y", LineColor<1>());
    _plot_ft->AddGraph("Force.z", LineColor<2>());
    _plot_ft->AddGraph("Torque.x", LineColor<3>());
    _plot_ft->AddGraph("Torque.y", LineColor<4>());
    _plot_ft->AddGraph("Torque.z", LineColor<5>());
    _plot_ft->show();
    RegisterPlot(_plot_ft.get());
#endif

    // 데이터 연결
#ifdef USE_TRANSPORT_ECAL
    _sub_ft = std::make_unique<eCAL::protobuf::CSubscriber<dtproto::sensor_msgs::FtTimeStamped>>("Ft");
    _sub_ft->AddReceiveCallback(std::bind(
        &FtSensorDataHandler::OnRecvFtTimeStamped, this,
        std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4));
#endif
#ifdef USE_TRANSPORT_GRPC
    std::string ip;
    uint16_t port;
    GetServerAddress(ip, port);
    if (ip.empty() || port == 0)
    {
        ip = "127.0.0.1";
        port = 50071;
    }
    std::string svr_address = string_format("%s:%d", ip.c_str(), port);

    _sub_ft = std::make_unique<dt::DAQ::StateSubscriberGrpc<dtproto::sensor_msgs::FtTimeStamped>>("Ft", svr_address);
    std::function<void(dtproto::sensor_msgs::FtTimeStamped &)> handler = [this](dtproto::sensor_msgs::FtTimeStamped &msg)
    {
        this->OnRecvFtTimeStamped("", msg, 0, this->_data_seq++);
    };
    _sub_ft->RegMessageHandler(handler);

    _sub_reconnector_running = true;
    _sub_reconnector = std::thread([this]
                                   {
        while (this->_sub_reconnector_running)
        {
            if (!this->_sub_ft->IsRunning())
            {
                LOG(warn) << "Disconnected. Reconnecting to FtSensor data server...";
                this->_sub_ft->Reconnect();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } });
#endif
}

void FtSensorDataHandler::OnRecvFtTimeStamped(const char *topic_name,
                                              const dtproto::sensor_msgs::FtTimeStamped &msg,
                                              const long long time, const long long clock)
{
    uint32_t seq = msg.header().seq();
    if (seq != 0 && seq < _msg_seq)
    {
        if (ClearPlotData())
            _data_seq = 0;
    }
    _msg_seq = seq;

    // double curTime = clock * 1e-3;
    double curTime = msg.header().time_stamp().seconds() + msg.header().time_stamp().nanos() * 1e-9;
    if (clock == 0)
        _data_seq_zero_time = curTime;
    curTime -= _data_seq_zero_time;

    OnRecvFt(curTime, msg.ft());

#ifdef PRINT_PUB_SUB_INFO
    qDebug() << "------------------------------------------";
    qDebug() << " FtSensor ";
    qDebug() << "------------------------------------------";
    qDebug() << "topic name   : " << topic_name;
    qDebug() << "topic time   : " << time;
    qDebug() << "topic clock  : " << clock;
    qDebug() << "------------------------------------------";
    qDebug() << " Header ";
    qDebug() << "------------------------------------------";
    qDebug() << "seq          : " << msg.header().seq();
    qDebug() << "timestamp(s) : " << curTime;
    qDebug() << "------------------------------------------";
    qDebug() << "";
#else
    Q_UNUSED(topic_name);
    // Q_UNUSED(msg);
    Q_UNUSED(time);
    Q_UNUSED(clock);
#endif // PRINT_PUB_SUB_INFO
}

void FtSensorDataHandler::OnRecvFt(const double curTime, const dtproto::sensor_msgs::Ft &ft)
{
    if (_plot_ft && (_plot_ft->isVisible() || IsFetchInvisibleData()))
    {
        _plot_ft->AddData(0, curTime, ft.force().x());
        _plot_ft->AddData(1, curTime, ft.force().y());
        _plot_ft->AddData(2, curTime, ft.force().z());
        _plot_ft->AddData(3, curTime, ft.torque().x());
        _plot_ft->AddData(4, curTime, ft.torque().y());
        _plot_ft->AddData(5, curTime, ft.torque().z());
        _plot_ft->DataUpdated(curTime);
    }
}

void FtSensorDataHandler::OnLoadTriggered(QString filename)
{
#ifndef USE_TRANSPORT_GRPC
    qDebug() << "MainWindow::OnLoadTriggered(): Not implemented!";
    return;
#endif

    mcap::McapReader reader;
    mcap::Status status;
    std::string fileName = filename.toStdString();
    // std::string topicName = "Ft";
    std::string schemaName = "dtproto.sensor_msgs.FtTimeStamped";

    if (!reader.open(fileName).ok())
    {
        qDebug() << "load file failed.";
        return;
    }

    auto messageView = reader.readMessages();
    mcap::Timestamp logTime_min{mcap::MaxTime}, logTime_max{0};

    for (auto itr = messageView.begin(); itr != messageView.end(); itr++)
    {
        if (itr->schema->encoding != "protobuf" || itr->schema->name != schemaName)
        {
            continue;
        }

        dtproto::sensor_msgs::FtTimeStamped message;
        if (!message.ParseFromArray(static_cast<const void *>(itr->message.data), itr->message.dataSize))
        {
            qDebug() << "could not parse " << schemaName.c_str();
            break;
        }

#ifdef USE_LOGGINGTIME_AS_TIMESTAMP
        // use message's logTime
        mcap::Timestamp logTime = itr->message.logTime;
        message.mutable_header()->mutable_time_stamp()->set_seconds((long)(logTime / 1000000000));
        message.mutable_header()->mutable_time_stamp()->set_nanos((long)(logTime % 1000000000));
#else
        // use message's timestamp
        mcap::Timestamp logTime = (uint64_t)(message.header().time_stamp().seconds() * 1000000000) + (uint64_t)message.header().time_stamp().nanos();
#endif

        if (logTime_min > logTime)
            logTime_min = logTime;
        if (logTime_max < logTime)
            logTime_max = logTime;

        // _msgs.push_back(message);
        OnRecvFtTimeStamped("", message, 0, _data_seq++);
    }

    reader.close();
}

void FtSensorDataHandler::OnClearTriggered()
{
    _data_seq = 0;
    _msg_seq = 0;
}