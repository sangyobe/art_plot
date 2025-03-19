#include "MotorAgingDataHandler.h"
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

#define ENABLE_JOINT_POSISION_PLOT
#define ENABLE_JOINT_VELOCITY_PLOT
// #define ENABLE_JOINT_ACCELERATION_PLOT
#define ENABLE_JOINT_TORQUE_PLOT
// #define ENABLE_JOINT_ABSOLUTE_ENCODER_PLOT
// #define ENABLE_JOINT_INCREMENTAL_ENCODER_PLOT
#define ENABLE_THREAD_STATE_PLOT

constexpr static int jdof = 12;
static const double RAD2DEG = 57.295779513; //! 180.0f/M_PI
static const double DEG2RAD = 0.0174532925; //! M_PI/180.0f

MotorAgingDataHandler::MotorAgingDataHandler(MainWindow *plotToolbox)
    : DataHandler(plotToolbox)
#ifdef ENABLE_JOINT_POSISION_PLOT
      ,
      _plot_jointPos(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_JOINT_VELOCITY_PLOT
      ,
      _plot_jointVel(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_JOINT_ACCELERATION_PLOT
      ,
      _plot_jointAcc(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_JOINT_TORQUE_PLOT
      ,
      _plot_jointTau(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_JOINT_INCREMENTAL_ENCODER_PLOT
      ,
      _plot_incEnc(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_JOINT_ABSOLUTE_ENCODER_PLOT
      ,
      _plot_absEnc(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_THREAD_STATE_PLOT
      ,
      _plot_threadState(std::make_unique<PlotWindow>(plotToolbox))
#endif
{
    LOG(info) << "MotorAgingDataHandler created.";

    BuildPlots();
}

MotorAgingDataHandler::~MotorAgingDataHandler()
{
#ifdef USE_TRANSPORT_GRPC
    _sub_reconnector_running = false;
    if (_sub_reconnector.joinable())
        _sub_reconnector.join();
#endif
}

void MotorAgingDataHandler::BuildPlots()
{
#ifdef ENABLE_JOINT_POSISION_PLOT
    //_plot_jointPos = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_jointPos->SetWindowTitle("Joint position");
    for (int ji = 0; ji < jdof; ji++)
    {
        _plot_jointPos->AddGraph(QString("Joint_%1.pos.desired").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji));
        _plot_jointPos->AddGraph(QString("Joint_%1.pos.actual").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji + jdof));
    }
    _plot_jointPos->show();
    RegisterPlot(_plot_jointPos.get());
#endif

#ifdef ENABLE_JOINT_VELOCITY_PLOT
    // _plot_jointVel = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_jointVel->SetWindowTitle("Joint velocity");
    for (int ji = 0; ji < jdof; ji++)
    {
        _plot_jointVel->AddGraph(QString("Joint_%1.vel.desired").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji));
        _plot_jointVel->AddGraph(QString("Joint_%1.vel.actual").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji + jdof));
    }
    _plot_jointVel->show();
    RegisterPlot(_plot_jointVel.get());
#endif

#ifdef ENABLE_JOINT_ACCELERATION_PLOT
    // _plot_jointAcc = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_jointAcc->SetWindowTitle("Joint acceleration");
    for (int ji = 0; ji < jdof; ji++)
    {
        _plot_jointAcc->AddGraph(QString("Joint_%1.acc.desired").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji));
        _plot_jointAcc->AddGraph(QString("Joint_%1.acc.actual").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji + jdof));
    }
    _plot_jointAcc->show();
    RegisterPlot(_plot_jointAcc.get());
    plotDataHandler.SetPlotJointAcc(_plot_jointAcc.get());
#endif

#ifdef ENABLE_JOINT_TORQUE_PLOT
    // _plot_jointTau = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_jointTau->SetWindowTitle("Joint torque");
    for (int ji = 0; ji < jdof; ji++)
    {
        _plot_jointTau->AddGraph(QString("Joint_%1.tau.desired").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji));
        _plot_jointTau->AddGraph(QString("Joint_%1.tau.actual").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji + jdof));
    }
    _plot_jointTau->show();
    RegisterPlot(_plot_jointTau.get());
#endif

#ifdef ENABLE_JOINT_ABSOLUTE_ENCODER_PLOT
    // _plot_absEnc = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_absEnc->SetWindowTitle("Joint absolute encoder");
    for (int ji = 0; ji < jdof; ji++)
    {
        _plot_absEnc->AddGraph(QString("Joint_%1.absolute_encoder").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji));
    }
    _plot_absEnc->show();
    RegisterPlot(_plot_absEnc.get());
#endif

#ifdef ENABLE_JOINT_INCREMENTAL_ENCODER_PLOT
    // _plot_incEnc = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_incEnc->SetWindowTitle("Joint incremental encoder");
    for (int ji = 0; ji < jdof; ji++)
    {
        _plot_incEnc->AddGraph(QString("Joint_%1.incremental_encoder").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji));
    }
    _plot_incEnc->show();
    RegisterPlot(_plot_incEnc.get());
#endif

#ifdef ENABLE_THREAD_STATE_PLOT
    // _plot_threadState = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_threadState->SetWindowTitle("Thread state");
    _plot_threadState->AddGraph("Control thread period(ms)", LineColor<0>());
    _plot_threadState->AddGraph("Control thread load(ms)", LineColor<1>());
    _plot_threadState->show();
    RegisterPlot(_plot_threadState.get());
#endif

    // 데이터 연결
#ifdef USE_TRANSPORT_ECAL
    _sub_state = std::make_unique<eCAL::protobuf::CSubscriber<dtproto::motor_aging::MotorAgingStateTimeStamped>>("RobotState");
    _sub_state->AddReceiveCallback(std::bind(
        &MotorAgingDataHandler::OnRecvLeoQuadStateTimeStamped, this,
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
        port = 50051;
    }
    std::string svr_address = string_format("%s:%d", ip.c_str(), port);

    _sub_state = std::make_unique<dt::DAQ::StateSubscriberGrpc<dtproto::motor_aging::MotorAgingStateTimeStamped>>("ControlState", svr_address);
    std::function<void(dtproto::motor_aging::MotorAgingStateTimeStamped &)> handler = [this](dtproto::motor_aging::MotorAgingStateTimeStamped &msg) {
        this->OnRecvMotorAgingStateTimeStamped("", msg, 0, this->_data_seq++);
    };
    _sub_state->RegMessageHandler(handler);

    _sub_reconnector_running = true;
    _sub_reconnector = std::thread([this] {
        while (this->_sub_reconnector_running)
        {
            if (!this->_sub_state->IsRunning())
            {
                LOG(warn) << "Disconnected. Reconnecting to MotorAgingState data server...";
                this->_sub_state->Reconnect();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } });
#endif
}

void MotorAgingDataHandler::OnRecvMotorAgingStateTimeStamped(const char *topic_name,
                                                             const dtproto::motor_aging::MotorAgingStateTimeStamped &state,
                                                             const long long time, const long long clock)
{
    double curTime = clock * 1e-3;

    uint32_t seq = state.header().seq();
    if (seq != 0 && seq < _msg_seq)
    {
        if (ClearPlotData())
            _data_seq = 0;
    }
    _msg_seq = seq;

    OnRecvMotorAgingState(curTime, state.state());

#ifdef PRINT_PUB_SUB_INFO
    qDebug() << "------------------------------------------";
    qDebug() << " QuadIP State ";
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

void MotorAgingDataHandler::OnRecvMotorAgingState(const double curTime, const dtproto::motor_aging::MotorAgingState &state)
{
    OnRecvJointState(curTime, state.jointstate(), state.actjointdata(), state.desjointdata());
    OnRecvThreadState(curTime, state.threadstate());
}

void MotorAgingDataHandler::OnRecvJointState(const double curTime,
                                             const google::protobuf::RepeatedPtrField<dtproto::motor_aging::JointState> &state,
                                             const google::protobuf::RepeatedPtrField<dtproto::motor_aging::JointData> &actData,
                                             const google::protobuf::RepeatedPtrField<dtproto::motor_aging::JointData> &desData)
{
    if (_plot_jointPos)
    {
        for (int ji = 0; ji < jdof; ji++)
        {
            _plot_jointPos->AddData(
                2 * ji, curTime, desData.Get(ji).pos_rad());
            _plot_jointPos->AddData(
                2 * ji + 1, curTime, actData.Get(ji).pos_rad());
        }
        _plot_jointPos->DataUpdated(curTime);
    }
    if (_plot_jointVel)
    {
        for (int ji = 0; ji < jdof; ji++)
        {
            _plot_jointVel->AddData(
                2 * ji, curTime, desData.Get(ji).vel_rps());
            _plot_jointVel->AddData(
                2 * ji + 1, curTime, actData.Get(ji).vel_rps());
        }
        _plot_jointVel->DataUpdated(curTime);
    }
    if (_plot_jointAcc)
    {
        for (int ji = 0; ji < jdof; ji++)
        {
            _plot_jointAcc->AddData(
                2 * ji, curTime, desData.Get(ji).acc_rpss());
            _plot_jointAcc->AddData(
                2 * ji + 1, curTime, actData.Get(ji).acc_rpss());
        }
        _plot_jointAcc->DataUpdated(curTime);
    }
    if (_plot_jointTau)
    {
        for (int ji = 0; ji < jdof; ji++)
        {
            _plot_jointTau->AddData(
                2 * ji, curTime, desData.Get(ji).torq_nm());
            _plot_jointTau->AddData(
                2 * ji + 1, curTime, actData.Get(ji).torq_nm());
        }
        _plot_jointTau->DataUpdated(curTime);
    }
    if (_plot_absEnc)
    {
        for (int ji = 0; ji < jdof; ji++)
        {
            _plot_absEnc->AddData(ji, curTime, state.Get(ji).abspos_cnt());
        }
        _plot_absEnc->DataUpdated(curTime);
    }
    if (_plot_incEnc)
    {
        for (int ji = 0; ji < jdof; ji++)
        {
            _plot_incEnc->AddData(ji, curTime, state.Get(ji).incpos_cnt());
        }
        _plot_incEnc->DataUpdated(curTime);
    }
}

void MotorAgingDataHandler::OnRecvThreadState(const double curTime, const dtproto::motor_aging::ThreadState &state)
{
    if (_plot_threadState)
    {
        _plot_threadState->AddData(0, curTime, state.ctrlperiod_ms());
        _plot_threadState->AddData(1, curTime, state.ctrlload_ms());
        _plot_threadState->DataUpdated(curTime);
    }
}

void MotorAgingDataHandler::OnLoadTriggered(QString filename)
{
#ifndef ROBOT_MOTORAGING
    qDebug() << "MainWindow::OnLoadTriggered(): Not implemented!";
    return;
#endif
#ifndef USE_TRANSPORT_GRPC
    qDebug() << "MainWindow::OnLoadTriggered(): Not implemented!";
    return;
#endif

    mcap::McapReader reader;
    mcap::Status status;
    std::string fileName = filename.toStdString();
    // std::string topicName = "MotorAgingState";
    std::string schemaName = "dtproto.motor_aging.MotorAgingStateTimeStamped";

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

        dtproto::motor_aging::MotorAgingStateTimeStamped message;
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
        OnRecvMotorAgingStateTimeStamped("", message, 0, _data_seq++);
    }

    reader.close();
}

void MotorAgingDataHandler::OnClearTriggered()
{
    _data_seq = 0;
    _msg_seq = 0;
}