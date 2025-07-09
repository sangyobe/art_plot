#include "DualArmDataHandler.h"
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
#define ENABLE_TASK_POSISION_PLOT
#define ENABLE_TASK_VELOCITY_PLOT
// #define ENABLE_TASK_TORQUE_PLOT
// #define ENABLE_JOINT_ABSOLUTE_ENCODER_PLOT
// #define ENABLE_JOINT_INCREMENTAL_ENCODER_PLOT
#define ENABLE_THREAD_STATE_PLOT
#define ENABLE_DEBUG_DATA_PLOT
#define ENABLE_DEBUG_DATA_XY_PLOT

constexpr static int jdof = 17;
constexpr static int armnum = 2;
constexpr static int tasknum = 2;

static std::string jointgroupname[jdof] = {
    "Torso", "Torso", "Torso",
    "RArm", "RArm", "RArm", "RArm", "RArm", "RArm", "RArm",
    "LArm", "LArm", "LArm", "LArm", "LArm", "LArm", "LArm"};
static std::string jointname[jdof] = {
    "T1", "T2", "T3",
    "R1", "R2", "R3", "R4", "R5", "R6", "R7",
    "L1", "L2", "L3", "L4", "L5", "L6", "L7"};
static std::string armname[armnum] = {"RArm", "LArm"};
static std::string taskname[armnum] = {"eeR", "eeL"};
static const double RAD2DEG = 57.295779513; //! 180.0f/M_PI
static const double DEG2RAD = 0.0174532925; //! M_PI/180.0f

DualArmDataHandler::DualArmDataHandler(MainWindow *plotToolbox)
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
#ifdef ENABLE_TASK_POSISION_PLOT
      ,
      _plot_taskPos(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_TASK_VELOCITY_PLOT
      ,
      _plot_taskVel(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_TASK_TORQUE_PLOT
      ,
      _plot_taskTau(std::make_unique<PlotWindow>(plotToolbox))
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
#ifdef ENABLE_DEBUG_DATA_PLOT
      ,
      _plot_debugData(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_DEBUG_DATA_XY_PLOT
      ,
      _plot_debugDataXY(std::make_unique<PlotWindow>(plotToolbox))
#endif
{
    LOG(info) << "DualArmDataHandler created.";

    BuildPlots();
}

DualArmDataHandler::~DualArmDataHandler()
{
#ifdef USE_TRANSPORT_GRPC
    _sub_reconnector_running = false;
    if (_sub_reconnector.joinable())
        _sub_reconnector.join();
#endif
}

void DualArmDataHandler::BuildPlots()
{
#ifdef ENABLE_JOINT_POSISION_PLOT
    //_plot_jointPos = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_jointPos->SetWindowTitle("Joint position");
    for (int ji = 0; ji < jdof; ji++)
    {
        _plot_jointPos->AddGraph((QString(jointname[ji].c_str()) + ".pos.desired"), LineColor(ji), jointgroupname[ji].c_str());
        _plot_jointPos->AddGraph((QString(jointname[ji].c_str()) + ".pos.actual"), LineColor(ji + jdof), jointgroupname[ji].c_str());
    }

    _plot_jointPos->show();
    RegisterPlot(_plot_jointPos.get());
#endif

#ifdef ENABLE_JOINT_VELOCITY_PLOT
    // _plot_jointVel = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_jointVel->SetWindowTitle("Joint velocity");
    for (int ji = 0; ji < jdof; ji++)
    {
        _plot_jointVel->AddGraph((QString(jointname[ji].c_str()) + ".vel.desired"), LineColor(ji), jointgroupname[ji].c_str());
        _plot_jointVel->AddGraph((QString(jointname[ji].c_str()) + ".vel.actual"), LineColor(ji + jdof), jointgroupname[ji].c_str());
    }
    _plot_jointVel->show();
    RegisterPlot(_plot_jointVel.get());
#endif

#ifdef ENABLE_JOINT_ACCELERATION_PLOT
    // _plot_jointAcc = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_jointAcc->SetWindowTitle("Joint acceleration");
    for (int ji = 0; ji < jdof; ji++)
    {
        _plot_jointAcc->AddGraph(QString("Joint_%1.acc.desired").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji), jointgroupname[ji].c_str());
        _plot_jointAcc->AddGraph(QString("Joint_%1.acc.actual").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji + jdof), jointgroupname[ji].c_str());
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
        _plot_jointTau->AddGraph((QString(jointname[ji].c_str()) + ".toq.desired"), LineColor(ji), jointgroupname[ji].c_str());
        _plot_jointTau->AddGraph((QString(jointname[ji].c_str()) + ".toq.actual"), LineColor(ji + jdof), jointgroupname[ji].c_str());
    }
    _plot_jointTau->show();
    RegisterPlot(_plot_jointTau.get());
#endif

#ifdef ENABLE_TASK_POSISION_PLOT
    //_plot_taskPos = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_taskPos->SetWindowTitle("Task position");
    for (int ti = 0; ti < tasknum; ti++)
    {
        _plot_taskPos->AddGraph((QString(taskname[ti].c_str()) + ".pos.x.desired"), LineColor(ti + 0), armname[ti].c_str());
        _plot_taskPos->AddGraph((QString(taskname[ti].c_str()) + ".pos.y.desired"), LineColor(ti + 1), armname[ti].c_str());
        _plot_taskPos->AddGraph((QString(taskname[ti].c_str()) + ".pos.z.desired"), LineColor(ti + 2), armname[ti].c_str());
        _plot_taskPos->AddGraph((QString(taskname[ti].c_str()) + ".rot.x.desired"), LineColor(ti + 3), armname[ti].c_str());
        _plot_taskPos->AddGraph((QString(taskname[ti].c_str()) + ".rot.y.desired"), LineColor(ti + 4), armname[ti].c_str());
        _plot_taskPos->AddGraph((QString(taskname[ti].c_str()) + ".rot.z.desired"), LineColor(ti + 5), armname[ti].c_str());
        _plot_taskPos->AddGraph((QString(taskname[ti].c_str()) + ".pos.x.actual"), LineColor(ti + 6 + tasknum * 12), armname[ti].c_str());
        _plot_taskPos->AddGraph((QString(taskname[ti].c_str()) + ".pos.y.actual"), LineColor(ti + 7 + tasknum * 12), armname[ti].c_str());
        _plot_taskPos->AddGraph((QString(taskname[ti].c_str()) + ".pos.z.actual"), LineColor(ti + 8 + tasknum * 12), armname[ti].c_str());
        _plot_taskPos->AddGraph((QString(taskname[ti].c_str()) + ".rot.x.actual"), LineColor(ti + 9 + tasknum * 12), armname[ti].c_str());
        _plot_taskPos->AddGraph((QString(taskname[ti].c_str()) + ".rot.y.actual"), LineColor(ti + 10 + tasknum * 12), armname[ti].c_str());
        _plot_taskPos->AddGraph((QString(taskname[ti].c_str()) + ".rot.z.actual"), LineColor(ti + 11 + tasknum * 12), armname[ti].c_str());
    }

    _plot_taskPos->show();
    RegisterPlot(_plot_taskPos.get());
#endif

#ifdef ENABLE_TASK_VELOCITY_PLOT
    //_plot_taskVel = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_taskVel->SetWindowTitle("Task velocity");
    for (int ti = 0; ti < tasknum; ti++)
    {
        _plot_taskVel->AddGraph((QString(taskname[ti].c_str()) + ".lin_vel.x.desired"), LineColor(ti + 0), armname[ti].c_str());
        _plot_taskVel->AddGraph((QString(taskname[ti].c_str()) + ".lin_vel.y.desired"), LineColor(ti + 1), armname[ti].c_str());
        _plot_taskVel->AddGraph((QString(taskname[ti].c_str()) + ".lin_vel.z.desired"), LineColor(ti + 2), armname[ti].c_str());
        _plot_taskVel->AddGraph((QString(taskname[ti].c_str()) + ".ang_vel.x.desired"), LineColor(ti + 3), armname[ti].c_str());
        _plot_taskVel->AddGraph((QString(taskname[ti].c_str()) + ".ang_vel.y.desired"), LineColor(ti + 4), armname[ti].c_str());
        _plot_taskVel->AddGraph((QString(taskname[ti].c_str()) + ".ang_vel.z.desired"), LineColor(ti + 5), armname[ti].c_str());
        _plot_taskVel->AddGraph((QString(taskname[ti].c_str()) + ".lin_vel.x.actual"), LineColor(ti + 6 + tasknum * 12), armname[ti].c_str());
        _plot_taskVel->AddGraph((QString(taskname[ti].c_str()) + ".lin_vel.y.actual"), LineColor(ti + 7 + tasknum * 12), armname[ti].c_str());
        _plot_taskVel->AddGraph((QString(taskname[ti].c_str()) + ".lin_vel.z.actual"), LineColor(ti + 8 + tasknum * 12), armname[ti].c_str());
        _plot_taskVel->AddGraph((QString(taskname[ti].c_str()) + ".ang_vel.x.actual"), LineColor(ti + 9 + tasknum * 12), armname[ti].c_str());
        _plot_taskVel->AddGraph((QString(taskname[ti].c_str()) + ".ang_vel.y.actual"), LineColor(ti + 10 + tasknum * 12), armname[ti].c_str());
        _plot_taskVel->AddGraph((QString(taskname[ti].c_str()) + ".ang_vel.z.actual"), LineColor(ti + 11 + tasknum * 12), armname[ti].c_str());
    }

    _plot_taskVel->show();
    RegisterPlot(_plot_taskVel.get());
#endif

#ifdef ENABLE_TASK_TORQUE_PLOT
    //_plot_taskTau = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_taskTau->SetWindowTitle("Task torque");
    for (int ti = 0; ti < tasknum; ti++)
    {
        _plot_taskTau->AddGraph((QString(taskname[ti].c_str()) + ".force.x.desired"), LineColor(ti + 0), armname[ti].c_str());
        _plot_taskTau->AddGraph((QString(taskname[ti].c_str()) + ".force.y.desired"), LineColor(ti + 1), armname[ti].c_str());
        _plot_taskTau->AddGraph((QString(taskname[ti].c_str()) + ".force.z.desired"), LineColor(ti + 2), armname[ti].c_str());
        _plot_taskTau->AddGraph((QString(taskname[ti].c_str()) + ".torque.x.desired"), LineColor(ti + 3), armname[ti].c_str());
        _plot_taskTau->AddGraph((QString(taskname[ti].c_str()) + ".torque.y.desired"), LineColor(ti + 4), armname[ti].c_str());
        _plot_taskTau->AddGraph((QString(taskname[ti].c_str()) + ".torque.z.desired"), LineColor(ti + 5), armname[ti].c_str());
        _plot_taskTau->AddGraph((QString(taskname[ti].c_str()) + ".force.x.actual"), LineColor(ti + 6 + tasknum * 12), armname[ti].c_str());
        _plot_taskTau->AddGraph((QString(taskname[ti].c_str()) + ".force.y.actual"), LineColor(ti + 7 + tasknum * 12), armname[ti].c_str());
        _plot_taskTau->AddGraph((QString(taskname[ti].c_str()) + ".force.z.actual"), LineColor(ti + 8 + tasknum * 12), armname[ti].c_str());
        _plot_taskTau->AddGraph((QString(taskname[ti].c_str()) + ".torque.x.actual"), LineColor(ti + 9 + tasknum * 12), armname[ti].c_str());
        _plot_taskTau->AddGraph((QString(taskname[ti].c_str()) + ".torque.y.actual"), LineColor(ti + 10 + tasknum * 12), armname[ti].c_str());
        _plot_taskTau->AddGraph((QString(taskname[ti].c_str()) + ".torque.z.actual"), LineColor(ti + 11 + tasknum * 12), armname[ti].c_str());
    }

    _plot_taskTau->show();
    RegisterPlot(_plot_taskTau.get());
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

#ifdef ENABLE_DEBUG_DATA_PLOT
    _debug_data_array_size_max = GetDebugDataNum();
    //_plot_debugData = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_debugData->SetWindowTitle("Debug Data Array");
    for (int gi = 0; gi < _debug_data_array_size_max; gi++)
    {
        _plot_debugData->AddGraph(QString("Data_%1").arg(gi, 2, 10, QLatin1Char('0')), LineColor(gi));
    }
    _plot_debugData->show();
    RegisterPlot(_plot_debugData.get());
#endif

#ifdef ENABLE_DEBUG_DATA_XY_PLOT
    _debug_data_xy_array_size_max = GetDebugDataXYNum();
    //_plot_debugDataXY = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_debugDataXY->SetWindowTitle("Debug XY Data Array");
    for (int gi = 0; gi < _debug_data_xy_array_size_max; gi++)
    {
        _plot_debugDataXY->AddGraph(QString("Data_%1").arg(gi, 2, 10, QLatin1Char('0')), LineColor(gi), QString(""), 0, LineScatterShape::ssStar, 0);
    }
    _plot_debugDataXY->show();
    RegisterPlot(_plot_debugDataXY.get());
#endif

    // 데이터 연결
#ifdef USE_TRANSPORT_ECAL
    _sub_state = std::make_unique<eCAL::protobuf::CSubscriber<dtproto::dualarm::DualArmStateTimeStamped>>("RobotState");
    _sub_state->AddReceiveCallback(std::bind(
        &DualArmDataHandler::OnRecvDualArmStateTimeStamped, this,
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

    _sub_state = std::make_unique<dt::DAQ::StateSubscriberGrpc<dtproto::dualarm::DualArmStateTimeStamped>>("RobotState", svr_address);
    std::function<void(dtproto::dualarm::DualArmStateTimeStamped &)> handler = [this](dtproto::dualarm::DualArmStateTimeStamped &msg)
    {
        this->OnRecvDualArmStateTimeStamped("", msg, 0, this->_data_seq++);
    };
    _sub_state->RegMessageHandler(handler);

    _sub_reconnector_running = true;
    _sub_reconnector = std::thread([this]
                                   {
        while (this->_sub_reconnector_running)
        {
            if (!this->_sub_state->IsRunning())
            {
                LOG(warn) << "Disconnected. Reconnecting to DualArmState data server...";
                this->_sub_state->Reconnect();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } });
#endif
}

void DualArmDataHandler::OnRecvDualArmStateTimeStamped(const char *topic_name,
                                                       const dtproto::dualarm::DualArmStateTimeStamped &state,
                                                       const long long time, const long long clock)
{
    uint32_t seq = state.header().seq();
    if (seq != 0 && seq < _msg_seq)
    {
        if (ClearPlotData())
            _data_seq = 0;
    }
    _msg_seq = seq;

    // double curTime = clock * 1e-3;
    double curTime = state.header().time_stamp().seconds() + state.header().time_stamp().nanos() * 1e-9;
    if (clock == 0)
        _data_seq_zero_time = curTime;
    curTime -= _data_seq_zero_time;

    OnRecvDualArmState(curTime, state.state());

#ifdef PRINT_PUB_SUB_INFO
    qDebug() << "------------------------------------------";
    qDebug() << " DualArm Control State ";
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

void DualArmDataHandler::OnRecvDualArmState(const double curTime, const dtproto::dualarm::DualArmState &state)
{
    OnRecvControlState(curTime, state.actcontrolstate(), state.descontrolstate());
    OnRecvJointState(curTime, state.jointstate(), state.actjointdata(), state.desjointdata());
    OnRecvTaskState(curTime, state.acttaskstate(), state.destaskstate());
    OnRecvThreadState(curTime, state.threadstate());
    OnRecvArbitraryState(curTime, state.arbitrarystate());
    OnRecvArbitraryStateXY(curTime, state.arbitrarystatexy());
}

void DualArmDataHandler::OnRecvControlState(const double curTime, const dtproto::dualarm::ControlState &actState, const dtproto::dualarm::ControlState &desState)
{
}

void DualArmDataHandler::OnRecvJointState(const double curTime,
                                          const google::protobuf::RepeatedPtrField<dtproto::dualarm::JointState> &state,
                                          const google::protobuf::RepeatedPtrField<dtproto::dualarm::JointData> &actData,
                                          const google::protobuf::RepeatedPtrField<dtproto::dualarm::JointData> &desData)
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

void DualArmDataHandler::OnRecvTaskState(const double curTime,
                                         const google::protobuf::RepeatedPtrField<dtproto::dualarm::TaskState> &actState,
                                         const google::protobuf::RepeatedPtrField<dtproto::dualarm::TaskState> &desState)
{
    if (_plot_taskPos)
    {
        for (int ti = 0; ti < tasknum; ti++)
        {
            _plot_taskPos->AddData(ti * 12 + 0, curTime, desState.Get(ti).pos().x());
            _plot_taskPos->AddData(ti * 12 + 1, curTime, desState.Get(ti).pos().y());
            _plot_taskPos->AddData(ti * 12 + 2, curTime, desState.Get(ti).pos().z());
            _plot_taskPos->AddData(ti * 12 + 3, curTime, desState.Get(ti).rot().x());
            _plot_taskPos->AddData(ti * 12 + 4, curTime, desState.Get(ti).rot().y());
            _plot_taskPos->AddData(ti * 12 + 5, curTime, desState.Get(ti).rot().z());
            _plot_taskPos->AddData(ti * 12 + 6, curTime, actState.Get(ti).pos().x());
            _plot_taskPos->AddData(ti * 12 + 7, curTime, actState.Get(ti).pos().y());
            _plot_taskPos->AddData(ti * 12 + 8, curTime, actState.Get(ti).pos().z());
            _plot_taskPos->AddData(ti * 12 + 9, curTime, actState.Get(ti).rot().x());
            _plot_taskPos->AddData(ti * 12 + 10, curTime, actState.Get(ti).rot().y());
            _plot_taskPos->AddData(ti * 12 + 11, curTime, actState.Get(ti).rot().z());
        }
        _plot_taskPos->DataUpdated(curTime);
    }
    if (_plot_taskVel)
    {
        for (int ti = 0; ti < tasknum; ti++)
        {
            _plot_taskVel->AddData(ti * 12 + 0, curTime, desState.Get(ti).linvel().x());
            _plot_taskVel->AddData(ti * 12 + 1, curTime, desState.Get(ti).linvel().y());
            _plot_taskVel->AddData(ti * 12 + 2, curTime, desState.Get(ti).linvel().z());
            _plot_taskVel->AddData(ti * 12 + 3, curTime, desState.Get(ti).angvel().x());
            _plot_taskVel->AddData(ti * 12 + 4, curTime, desState.Get(ti).angvel().y());
            _plot_taskVel->AddData(ti * 12 + 5, curTime, desState.Get(ti).angvel().z());
            _plot_taskVel->AddData(ti * 12 + 6, curTime, actState.Get(ti).linvel().x());
            _plot_taskVel->AddData(ti * 12 + 7, curTime, actState.Get(ti).linvel().y());
            _plot_taskVel->AddData(ti * 12 + 8, curTime, actState.Get(ti).linvel().z());
            _plot_taskVel->AddData(ti * 12 + 9, curTime, actState.Get(ti).angvel().x());
            _plot_taskVel->AddData(ti * 12 + 10, curTime, actState.Get(ti).angvel().y());
            _plot_taskVel->AddData(ti * 12 + 11, curTime, actState.Get(ti).angvel().z());
        }
        _plot_taskVel->DataUpdated(curTime);
    }
    if (_plot_taskTau)
    {
        for (int ti = 0; ti < tasknum; ti++)
        {
            _plot_taskTau->AddData(ti * 12 + 0, curTime, desState.Get(ti).force().x());
            _plot_taskTau->AddData(ti * 12 + 1, curTime, desState.Get(ti).force().y());
            _plot_taskTau->AddData(ti * 12 + 2, curTime, desState.Get(ti).force().z());
            _plot_taskTau->AddData(ti * 12 + 3, curTime, desState.Get(ti).moment().x());
            _plot_taskTau->AddData(ti * 12 + 4, curTime, desState.Get(ti).moment().y());
            _plot_taskTau->AddData(ti * 12 + 5, curTime, desState.Get(ti).moment().z());
            _plot_taskTau->AddData(ti * 12 + 6, curTime, actState.Get(ti).force().x());
            _plot_taskTau->AddData(ti * 12 + 7, curTime, actState.Get(ti).force().y());
            _plot_taskTau->AddData(ti * 12 + 8, curTime, actState.Get(ti).force().z());
            _plot_taskTau->AddData(ti * 12 + 9, curTime, actState.Get(ti).moment().x());
            _plot_taskTau->AddData(ti * 12 + 10, curTime, actState.Get(ti).moment().y());
            _plot_taskTau->AddData(ti * 12 + 11, curTime, actState.Get(ti).moment().z());
        }
        _plot_taskTau->DataUpdated(curTime);
    }
}

void DualArmDataHandler::OnRecvThreadState(const double curTime, const dtproto::dualarm::ThreadState &state)
{
    if (_plot_threadState)
    {
        _plot_threadState->AddData(0, curTime, state.ctrlperiod_ms());
        _plot_threadState->AddData(1, curTime, state.ctrlload_ms());
        _plot_threadState->DataUpdated(curTime);
    }
}

void DualArmDataHandler::OnRecvArbitraryState(const double curTime, const dtproto::std_msgs::PackedDouble &state)
{
    if (!_plot_debugData || state.GetTypeName() != "dtproto.std_msgs.PackedDouble")
    {
        return;
    }

    if (_debug_data_array_size < 0)
    {
        _debug_data_array_size = state.data_size();
        if (_debug_data_array_size > _debug_data_array_size_max)
            _debug_data_array_size = _debug_data_array_size_max;

        // data handler는 GUI thread와 다른 thread --> runtime warning 발생한다.
        // for (int gi = 0; gi < _debug_data_array_size; gi++) {
        //     _plot_debugData->AddGraph(QString("Data_%1.abs").arg(gi, 2, 10, QLatin1Char('0')), LineColor(gi));
        // }
    }

    for (int gi = 0; gi < _debug_data_array_size; gi++)
    {
        if (gi >= _debug_data_array_size_max || gi >= _plot_debugData->GetGraphCount())
            return;

        // LOG(trace) << "  data[" << gi << "] = " << state.data(gi);
        _plot_debugData->AddData(gi, curTime, state.data().Get(gi));
        _plot_debugData->DataUpdated(curTime);
    }
}

void DualArmDataHandler::OnRecvArbitraryStateXY(const double curTime, const dtproto::std_msgs::PackedDouble &state)
{
    if (!_plot_debugDataXY || state.GetTypeName() != "dtproto.std_msgs.PackedDouble")
    {
        return;
    }

    if (_debug_data_xy_array_size < 0)
    {
        _debug_data_xy_array_size = state.data_size() / 2;
        if (_debug_data_xy_array_size > _debug_data_xy_array_size_max)
            _debug_data_xy_array_size = _debug_data_xy_array_size_max;
    }

    for (int gi = 0; gi < _debug_data_xy_array_size; gi++)
    {
        if (gi >= _debug_data_xy_array_size_max || gi >= _plot_debugDataXY->GetGraphCount())
            return;

        // LOG(trace) << "  data[" << gi << "] = " << state.data(2*gi) << ", " << << state.data(2*gi+1);
        _plot_debugDataXY->AddData(gi, state.data().Get(2 * gi), state.data().Get(2 * gi + 1));
    }
}

void DualArmDataHandler::OnLoadTriggered(QString filename)
{
#ifndef ROBOT_DUALARM
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
    // std::string topicName = "DualArmState";
    std::string schemaName = "dtproto.dualarm.DualArmStateTimeStamped";

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

        dtproto::dualarm::DualArmStateTimeStamped message;
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
        OnRecvDualArmStateTimeStamped("", message, 0, _data_seq++);
    }

    reader.close();
}

void DualArmDataHandler::OnClearTriggered()
{
    _data_seq = 0;
    _msg_seq = 0;
}