#include "G1DataHandler.h"
#include "cmdopts.hpp"
#include "mainwindow.h"
#include "pconstants.h"
#include "plotwindow.h"
#include "plotcentralwidget.h"
#include "stringutil.hpp"
#include <QApplication>
#include <QDebug>
#include <chrono>
#include <dtCore/src/dtLog/dtLog.h>
#include <dtMath/dtMath.h>

#define MCAP_COMPRESSION_NO_LZ4
#define MCAP_COMPRESSION_NO_ZSTD
#include <mcap/mcap.hpp>

#define USE_LOGGINGTIME_AS_TIMESTAMP

// Enable plot types for G1
#define ENABLE_JOINT_POSITION_PLOT
#define ENABLE_JOINT_VELOCITY_PLOT
#define ENABLE_JOINT_ACCELERATION_PLOT
#define ENABLE_JOINT_TORQUE_PLOT
#define ENABLE_THREAD_STATE_PLOT
// #define ENABLE_DEBUG_DATA_PLOT
// #define ENABLE_DEBUG_DATA_XY_PLOT
#define ENABLE_PELVIS_IMU_PLOT
#define ENABLE_TORSO_IMU_PLOT


#ifdef ENABLE_JOINT_POSITION_PLOT
    #define ENABLE_UPPER_JOINT_POSITION_PLOT
    #define ENABLE_LOWER_JOINT_POSITION_PLOT
#endif

#ifdef ENABLE_JOINT_VELOCITY_PLOT
    #define ENABLE_UPPER_JOINT_VELOCITY_PLOT
    #define ENABLE_LOWER_JOINT_VELOCITY_PLOT
#endif

#ifdef ENABLE_JOINT_ACCELERATION_PLOT
    #define ENABLE_UPPER_JOINT_ACCELERATION_PLOT
    #define ENABLE_LOWER_JOINT_ACCELERATION_PLOT
#endif

#ifdef ENABLE_JOINT_TORQUE_PLOT
    #define ENABLE_UPPER_JOINT_TORQUE_PLOT
    #define ENABLE_LOWER_JOINT_TORQUE_PLOT
#endif

constexpr static int jdof = 29; // Adjust based on your robot's DOF
constexpr static int lowdof = 12; // Adjust based on your robot's DOF
constexpr static int updof = 17; // Adjust based on your robot's DOF

static std::string jointname[29] = {"LHP", "LHR", "LHY", "LKN", "LAP", "LAR", "RHP", "RHR", "RHY", "RKN", "RAP", "RAR", "WAY", "WAR", "WAP", "LSP", "LSR", "LSY", "LEB", "LWR", "LWP", "LWY", "RSP", "RSR", "RSY", "REB", "RWR", "RWP", "RWY"};



G1DataHandler::G1DataHandler(MainWindow *plotToolbox)
    : DataHandler(plotToolbox, "g1"), _plotToolbox(plotToolbox)
{
    // qDebug() << "G1DataHandler: Created.";

    // Build plot windows
    BuildPlots();

    // Initialize gRPC subscriber
#ifdef USE_TRANSPORT_GRPC
    std::string ip;
    uint16_t port;
    GetServerAddress(ip, port);
    if (ip.empty() || port == 0)
    {
        ip = "127.0.0.1";
        port = 50051;
    }
    std::string server_address = ip + ":" + std::to_string(port);
    _sub_state = std::make_unique<dt::DAQ::StateSubscriberGrpc<dtproto::g1::G1StateTimeStamped>>(
        "G1State", server_address);

    // Register message handler for gRPC subscriber
    std::function<void(dtproto::g1::G1StateTimeStamped &)> handler = [this](dtproto::g1::G1StateTimeStamped &msg)
    {
        this->OnRecvG1StateTimeStamped("", msg, 0, this->_data_seq++);
    };
    _sub_state->RegMessageHandler(handler);

    _sub_reconnector_running = true;
    _sub_reconnector = std::thread([this]
                                   {
        while (this->_sub_reconnector_running)
        {
            if (!this->_sub_state->IsRunning())
            {
                // qDebug() << "G1DataHandler: Disconnected. Reconnecting to G1State data server...";
                this->_sub_state->Reconnect();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } });

    // qDebug() << "G1DataHandler: gRPC subscriber created for" << server_address.c_str();
#endif

    // qDebug() << "G1DataHandler: Initialized.";
}

G1DataHandler::~G1DataHandler()
{
#ifdef USE_TRANSPORT_GRPC
    _sub_reconnector_running = false;
    if (_sub_reconnector.joinable())
        _sub_reconnector.join();
#endif
    // qDebug() << "G1DataHandler: Destroyed.";
}

void G1DataHandler::BuildPlots()
{
    // qDebug() << "G1DataHandler: BuildPlots() started.";
    QString groupName;
#ifdef ENABLE_THREAD_STATE_PLOT
    // qDebug() << "Creating Thread State plot...";
    _plot_threadState = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_threadState->SetWindowTitle("Thread State");
    _plot_threadState->AddGraph("Ctrl Period [ms]", Qt::red);
    _plot_threadState->AddGraph("Ctrl Load [ms]", Qt::blue);
    _plot_threadState->AddGraph("Rl Period [ms]", Qt::green);
    _plot_threadState->AddGraph("Rl Load [ms]", Qt::darkGreen);
    RegisterPlot(_plot_threadState.get());
    // qDebug() << "Thread State plot created successfully.";
#endif

#ifdef ENABLE_UPPER_JOINT_POSITION_PLOT
    // qDebug() << "Creating Upper Joint Position plot...";
    _plot_upperJointPos = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_upperJointPos->SetWindowTitle("Upper Joint Position");
    for (int i = lowdof; i < jdof; i++)
    {
        if(i < 15)  groupName = "Waist";
        else if(i < 22) groupName = "Left Arm";
        else groupName = "Right Arm";

        _plot_upperJointPos->AddGraph(QString("act%1 [rad]").arg(jointname[i].c_str()), LineColor(i * 2), groupName);
        _plot_upperJointPos->AddGraph(QString("des%1 [rad]").arg(jointname[i].c_str()), LineColor(i * 2 + 1), groupName);
    }
    RegisterPlot(_plot_upperJointPos.get());
    // qDebug() << "Upper Joint Position plot created successfully.";
#endif

#ifdef ENABLE_LOWER_JOINT_POSITION_PLOT
    // qDebug() << "Creating Lower Joint Position plot...";
    _plot_lowerJointPos = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_lowerJointPos->SetWindowTitle("Lower Joint Position");
    for (int i = 0; i < lowdof; i++)
    {
        if(i < 6)  groupName = "Left Leg";
        else groupName = "Right Leg";

        _plot_lowerJointPos->AddGraph(QString("act%1 [rad]").arg(jointname[i].c_str()), LineColor(i * 2), groupName);
        _plot_lowerJointPos->AddGraph(QString("des%1 [rad]").arg(jointname[i].c_str()), LineColor(i * 2 + 1), groupName);
    }
    RegisterPlot(_plot_lowerJointPos.get());
    // qDebug() << "Lower Joint Position plot created successfully.";
#endif

#ifdef ENABLE_UPPER_JOINT_VELOCITY_PLOT
    // qDebug() << "Creating Upper Joint Velocity plot...";
    _plot_upperJointVel = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_upperJointVel->SetWindowTitle("Upper Joint Velocity");
    for (int i = lowdof; i < jdof; i++)
    {
        if(i < 15)  groupName = "Waist";
        else if(i < 22) groupName = "Left Arm";
        else groupName = "Right Arm";

        _plot_upperJointVel->AddGraph(QString("act %1 [rad/s]").arg(jointname[i].c_str()), LineColor(i * 2), groupName);
        _plot_upperJointVel->AddGraph(QString("des %1 [rad/s]").arg(jointname[i].c_str()), LineColor(i * 2 + 1), groupName);
    }
    RegisterPlot(_plot_upperJointVel.get());
    // qDebug() << "Upper Joint Velocity plot created successfully.";
#endif

#ifdef ENABLE_LOWER_JOINT_VELOCITY_PLOT
    // qDebug() << "Creating Lower Joint Velocity plot...";
    _plot_lowerJointVel = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_lowerJointVel->SetWindowTitle("Lower Joint Velocity");
    for (int i = 0; i < lowdof; i++)
    {
        if(i < 6)  groupName = "Left Leg";
        else groupName = "Right Leg";

        _plot_lowerJointVel->AddGraph(QString("act %1 [rad/s]").arg(jointname[i].c_str()), LineColor(i * 2), groupName);
        _plot_lowerJointVel->AddGraph(QString("des %1 [rad/s]").arg(jointname[i].c_str()), LineColor(i * 2 + 1), groupName);
    }
    RegisterPlot(_plot_lowerJointVel.get());
    // qDebug() << "Lower Joint Velocity plot created successfully.";
#endif

#ifdef ENABLE_UPPER_JOINT_ACCELERATION_PLOT
    // qDebug() << "Creating Upper Joint Acceleration plot...";
    _plot_upperJointAcc = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_upperJointAcc->SetWindowTitle("Upper Joint Acceleration");
    for (int i = lowdof; i < jdof; i++)
    {
        if(i < 15)  groupName = "Waist";
        else if(i < 22) groupName = "Left Arm";
        else groupName = "Right Arm";

        _plot_upperJointAcc->AddGraph(QString("act %1 [rad/s²]").arg(jointname[i].c_str()), LineColor(i * 2), groupName);
        _plot_upperJointAcc->AddGraph(QString("des %1 [rad/s²]").arg(jointname[i].c_str()), LineColor(i * 2 + 1), groupName);
    }
    RegisterPlot(_plot_upperJointAcc.get());
    // qDebug() << "Upper Joint Acceleration plot created successfully.";
#endif

#ifdef ENABLE_LOWER_JOINT_ACCELERATION_PLOT
    // qDebug() << "Creating Lower Joint Acceleration plot...";
    _plot_lowerJointAcc = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_lowerJointAcc->SetWindowTitle("Lower Joint Acceleration");
    for (int i = 0; i < lowdof; i++)
    {
        if(i < 6)  groupName = "Left Leg";
        else groupName = "Right Leg";

        _plot_lowerJointAcc->AddGraph(QString("act %1 [rad/s²]").arg(jointname[i].c_str()), LineColor(i * 2), groupName);
        _plot_lowerJointAcc->AddGraph(QString("des %1 [rad/s²]").arg(jointname[i].c_str()), LineColor(i * 2 + 1), groupName);
    }
    RegisterPlot(_plot_lowerJointAcc.get());
    // qDebug() << "Lower Joint Acceleration plot created successfully.";
#endif

#ifdef ENABLE_UPPER_JOINT_TORQUE_PLOT
    // qDebug() << "Creating Upper Joint Torque plot...";
    _plot_upperJointTau = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_upperJointTau->SetWindowTitle("Upper Joint Torque");
    for (int i = lowdof; i < jdof; i++)
    {
        if(i < 15)  groupName = "Waist";
        else if(i < 22) groupName = "Left Arm";
        else groupName = "Right Arm";

        _plot_upperJointTau->AddGraph(QString("act %1 [Nm]").arg(jointname[i].c_str()), LineColor(i * 2), groupName);
        _plot_upperJointTau->AddGraph(QString("des %1 [Nm]").arg(jointname[i].c_str()), LineColor(i * 2 + 1), groupName);
    }
    RegisterPlot(_plot_upperJointTau.get());
    // qDebug() << "Upper Joint Torque plot created successfully.";
#endif

#ifdef ENABLE_LOWER_JOINT_TORQUE_PLOT
    // qDebug() << "Creating Lower Joint Torque plot...";
    _plot_lowerJointTau = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_lowerJointTau->SetWindowTitle("Lower Joint Torque");
    for (int i = 0; i < lowdof; i++)
    {
        if(i < 6)  groupName = "Left Leg";
        else groupName = "Right Leg";

        _plot_lowerJointTau->AddGraph(QString("act %1 [Nm]").arg(jointname[i].c_str()), LineColor(i * 2), groupName);
        _plot_lowerJointTau->AddGraph(QString("des %1 [Nm]").arg(jointname[i].c_str()), LineColor(i * 2 + 1), groupName);
    }
    RegisterPlot(_plot_lowerJointTau.get());
    // qDebug() << "Lower Joint Torque plot created successfully.";
#endif

#ifdef ENABLE_PELVIS_IMU_PLOT
    // qDebug() << "Creating Pelvis IMU plot...";
    _plot_pelvisImu = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_pelvisImu->SetWindowTitle("Pelvis IMU");
    // Linear Acceleration
    _plot_pelvisImu->AddGraph("acc x [m/s²]", Qt::red);
    _plot_pelvisImu->AddGraph("acc y [m/s²]", Qt::green);
    _plot_pelvisImu->AddGraph("acc z [m/s²]", Qt::blue);
    // Angular Velocity
    _plot_pelvisImu->AddGraph("gyro x [rad/s]", Qt::darkRed);
    _plot_pelvisImu->AddGraph("gyro y [rad/s]", Qt::darkGreen);
    _plot_pelvisImu->AddGraph("gyro z [rad/s]", Qt::darkBlue);
    // Quaternion Orientation
    _plot_pelvisImu->AddGraph("quat w", Qt::magenta);
    _plot_pelvisImu->AddGraph("quat x", Qt::cyan);
    _plot_pelvisImu->AddGraph("quat y", Qt::yellow);
    _plot_pelvisImu->AddGraph("quat z", Qt::gray);
    RegisterPlot(_plot_pelvisImu.get());
    //qDebug() << "Pelvis IMU plot created successfully.";
#endif

#ifdef ENABLE_TORSO_IMU_PLOT
    //qDebug() << "Creating Torso IMU plot...";
    _plot_torsoImu = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_torsoImu->SetWindowTitle("Torso IMU");
    // Linear Acceleration
    _plot_torsoImu->AddGraph("acc x [m/s²]", Qt::red);
    _plot_torsoImu->AddGraph("acc y [m/s²]", Qt::green);
    _plot_torsoImu->AddGraph("acc z [m/s²]", Qt::blue);
    // Angular Velocity
    _plot_torsoImu->AddGraph("gyro x [rad/s]", Qt::darkRed);
    _plot_torsoImu->AddGraph("gyro y [rad/s]", Qt::darkGreen);
    _plot_torsoImu->AddGraph("gyro z [rad/s]", Qt::darkBlue);
    // Quaternion Orientation
    _plot_torsoImu->AddGraph("quat w", Qt::magenta);
    _plot_torsoImu->AddGraph("quat x", Qt::cyan);
    _plot_torsoImu->AddGraph("quat y", Qt::yellow);
    _plot_torsoImu->AddGraph("quat z", Qt::gray);
    RegisterPlot(_plot_torsoImu.get());
    //qDebug() << "Torso IMU plot created successfully.";
#endif

#ifdef ENABLE_DEBUG_DATA_PLOT
    _plot_debugData = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_debugData->SetWindowTitle("Debug Data");
    _debug_data_array_size_max = GetDebugDataNum();
    RegisterPlot(_plot_debugData.get());
#endif

#ifdef ENABLE_DEBUG_DATA_XY_PLOT
    _plot_debugDataXY = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_debugDataXY->SetWindowTitle("Debug Data XY");
    _debug_data_xy_array_size_max = GetDebugDataXYNum();
    RegisterPlot(_plot_debugDataXY.get());
#endif

    //qDebug() << "G1DataHandler: BuildPlots() finished.";
}

void G1DataHandler::OnRecvG1StateTimeStamped(const char *topic_name,
                                              const dtproto::g1::G1StateTimeStamped &state,
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

    // 실제 데이터 처리 함수 호출
    OnRecvG1State(curTime, state.state());

    _data_seq++;
}

void G1DataHandler::OnRecvG1State(const double curTime, const dtproto::g1::G1State &state)
{

        // Process thread state
    if (state.has_threadstate())
    {
        OnRecvThreadState(curTime, state.threadstate());
    }

    // Process joint state
    if (state.actjointdata_size() > 0)
    {
        OnRecvJointState(curTime, state.actjointdata(), state.desjointdata());
    }

        // Process IMU data
    if (state.has_pelvisimu())
    {
        OnRecvPelvisImu(curTime, state.pelvisimu());
    }

    if (state.has_torsoimu())
    {
        OnRecvTorsoImu(curTime, state.torsoimu());
    }

    // Process arbitrary debug data
    if (state.has_arbitrarystate())
    {
        OnRecvArbitraryState(curTime, state.arbitrarystate());
    }

    if (state.has_arbitrarystatexy())
    {
        OnRecvArbitraryStateXY(curTime, state.arbitrarystatexy());
    }
}

void G1DataHandler::OnRecvJointState(const double curTime,
                                     const google::protobuf::RepeatedPtrField<dtproto::g1::JointData> &actData,
                                     const google::protobuf::RepeatedPtrField<dtproto::g1::JointData> &desData)
{
    int jointNum = std::min({(int)actData.size(), (int)desData.size(), jdof});

#ifdef ENABLE_UPPER_JOINT_POSITION_PLOT
    // 상체 관절 위치 데이터 플롯 처리 (12번째 관절부터)
    if (_plot_upperJointPos && jointNum > lowdof)
    {
        for (int i = lowdof; i < jointNum && i < jdof; i++)
        {
            int graph_idx = (i - lowdof) * 2; // 각 관절마다 2개의 그래프 (실제값, 목표값)
            if (graph_idx < _plot_upperJointPos->GetGraphCount())
            {
                // 실제 관절 위치 데이터 추가
                _plot_upperJointPos->AddData(graph_idx, curTime, actData[i].pos_rad());
                // 목표 관절 위치 데이터 추가
                if (i < (int)desData.size() && (graph_idx + 1) < _plot_upperJointPos->GetGraphCount())
                    _plot_upperJointPos->AddData(graph_idx + 1, curTime, desData[i].pos_rad());
            }
            _plot_upperJointPos->DataUpdated(curTime);
        }
    }
#endif

#ifdef ENABLE_LOWER_JOINT_POSITION_PLOT
    // 하체 관절 위치 데이터 플롯 처리 (0~11번째 관절)
    if (_plot_lowerJointPos)
    {
        for(int i = 0; i < lowdof && i < jointNum; i++)
        {
            int graph_idx = i * 2;
            if (graph_idx < _plot_lowerJointPos->GetGraphCount())
            {
                // 실제 관절 위치 데이터 추가
                _plot_lowerJointPos->AddData(graph_idx, curTime, actData[i].pos_rad());
                // 목표 관절 위치 데이터 추가
                if (i < (int)desData.size() && (graph_idx + 1) < _plot_lowerJointPos->GetGraphCount())
                    _plot_lowerJointPos->AddData(graph_idx + 1, curTime, desData[i].pos_rad());
            }
        }
        _plot_lowerJointPos->DataUpdated(curTime);
    }
#endif

#ifdef ENABLE_UPPER_JOINT_VELOCITY_PLOT
    // 상체 관절 속도 데이터 플롯 처리
    if (_plot_upperJointVel && jointNum > lowdof)
    {
        for (int i = lowdof; i < jointNum && i < jdof; i++)
        {
            int graph_idx = (i - lowdof) * 2;
            if (graph_idx < _plot_upperJointVel->GetGraphCount())
            {
                _plot_upperJointVel->AddData(graph_idx, curTime, actData[i].vel_rps());
                if (i < (int)desData.size() && (graph_idx + 1) < _plot_upperJointVel->GetGraphCount())
                    _plot_upperJointVel->AddData(graph_idx + 1, curTime, desData[i].vel_rps());
            }
        }
        _plot_upperJointVel->DataUpdated(curTime);
    }
#endif

#ifdef ENABLE_LOWER_JOINT_VELOCITY_PLOT
    // 하체 관절 속도 데이터 플롯 처리
    if (_plot_lowerJointVel)
    {
        for (int i = 0; i < lowdof && i < jointNum; i++)
        {
            int graph_idx = i * 2;
            if (graph_idx < _plot_lowerJointVel->GetGraphCount())
            {
                _plot_lowerJointVel->AddData(graph_idx, curTime, actData[i].vel_rps());
                if (i < (int)desData.size() && (graph_idx + 1) < _plot_lowerJointVel->GetGraphCount())
                    _plot_lowerJointVel->AddData(graph_idx + 1, curTime, desData[i].vel_rps());
            }
        }
        _plot_lowerJointVel->DataUpdated(curTime);
    }
#endif

#ifdef ENABLE_UPPER_JOINT_ACCELERATION_PLOT
    // 상체 관절 가속도 데이터 플롯 처리
    if (_plot_upperJointAcc && jointNum > lowdof)
    {
        for (int i = lowdof; i < jointNum && i < jdof; i++)
        {
            int graph_idx = (i - lowdof) * 2;
            if (graph_idx < _plot_upperJointAcc->GetGraphCount())
            {
                _plot_upperJointAcc->AddData(graph_idx, curTime, actData[i].acc_rpss());
                if (i < (int)desData.size() && (graph_idx + 1) < _plot_upperJointAcc->GetGraphCount())
                    _plot_upperJointAcc->AddData(graph_idx + 1, curTime, desData[i].acc_rpss());
            }
        }
        _plot_upperJointAcc->DataUpdated(curTime);
    }
#endif

#ifdef ENABLE_LOWER_JOINT_ACCELERATION_PLOT
    // 하체 관절 가속도 데이터 플롯 처리
    if (_plot_lowerJointAcc)
    {
        for (int i = 0; i < lowdof && i < jointNum; i++)
        {
            int graph_idx = i * 2;
            if (graph_idx < _plot_lowerJointAcc->GetGraphCount())
            {
                _plot_lowerJointAcc->AddData(graph_idx, curTime, actData[i].acc_rpss());
                if (i < (int)desData.size() && (graph_idx + 1) < _plot_lowerJointAcc->GetGraphCount())
                    _plot_lowerJointAcc->AddData(graph_idx + 1, curTime, desData[i].acc_rpss());
            }
        }
        _plot_lowerJointAcc->DataUpdated(curTime);
    }
#endif

#ifdef ENABLE_UPPER_JOINT_TORQUE_PLOT
    // 상체 관절 토크 데이터 플롯 처리
    if (_plot_upperJointTau && jointNum > lowdof)
    {
        for (int i = lowdof; i < jointNum && i < jdof; i++)
        {
            int graph_idx = (i - lowdof) * 2;
            if (graph_idx < _plot_upperJointTau->GetGraphCount())
            {
                _plot_upperJointTau->AddData(graph_idx, curTime, actData[i].torq_nm());
                if (i < (int)desData.size() && (graph_idx + 1) < _plot_upperJointTau->GetGraphCount())
                    _plot_upperJointTau->AddData(graph_idx + 1, curTime, desData[i].torq_nm());
            }
        }
        _plot_upperJointTau->DataUpdated(curTime);
    }
#endif

#ifdef ENABLE_LOWER_JOINT_TORQUE_PLOT
    // 하체 관절 토크 데이터 플롯 처리
    if (_plot_lowerJointTau)
    {
        for (int i = 0; i < lowdof && i < jointNum; i++)
        {
            int graph_idx = i * 2;
            if (graph_idx < _plot_lowerJointTau->GetGraphCount())
            {
                _plot_lowerJointTau->AddData(graph_idx, curTime, actData[i].torq_nm());
                if (i < (int)desData.size() && (graph_idx + 1) < _plot_lowerJointTau->GetGraphCount())
                    _plot_lowerJointTau->AddData(graph_idx + 1, curTime, desData[i].torq_nm());
            }
        }
        _plot_lowerJointTau->DataUpdated(curTime);
    }
#endif
}

void G1DataHandler::OnRecvThreadState(const double curTime, const dtproto::g1::ThreadState &state)
{

#ifdef ENABLE_THREAD_STATE_PLOT
    if (_plot_threadState)
    {
        _plot_threadState->AddData(0, curTime, state.ctrlperiod_ms());
        _plot_threadState->AddData(1, curTime, state.ctrlload_ms());
        _plot_threadState->AddData(2, curTime, state.rlperiod_ms());
        _plot_threadState->AddData(3, curTime, state.rlload_ms());
        _plot_threadState->DataUpdated(curTime);
    }
    #endif
}

void G1DataHandler::OnRecvPelvisImu(const double curTime, const dtproto::sensor_msgs::Imu &imu)
{
#ifdef ENABLE_PELVIS_IMU_PLOT
    if (_plot_pelvisImu)
    {
        // Linear Acceleration (indices 0-2)
        _plot_pelvisImu->AddData(0, curTime, imu.linear_acceleration().a1());
        _plot_pelvisImu->AddData(1, curTime, imu.linear_acceleration().a2());
        _plot_pelvisImu->AddData(2, curTime, imu.linear_acceleration().a3());
        
        // Angular Velocity (indices 3-5)
        _plot_pelvisImu->AddData(3, curTime, imu.angular_velocity().a1());
        _plot_pelvisImu->AddData(4, curTime, imu.angular_velocity().a2());
        _plot_pelvisImu->AddData(5, curTime, imu.angular_velocity().a3());
        
        // Quaternion Orientation (indices 6-9)
        _plot_pelvisImu->AddData(6, curTime, imu.orientation().w());
        _plot_pelvisImu->AddData(7, curTime, imu.orientation().x());
        _plot_pelvisImu->AddData(8, curTime, imu.orientation().y());
        _plot_pelvisImu->AddData(9, curTime, imu.orientation().z());
        
        _plot_pelvisImu->DataUpdated(curTime);
    }
#endif
}


void G1DataHandler::OnRecvTorsoImu(const double curTime, const dtproto::sensor_msgs::Imu &imu)
{
#ifdef ENABLE_TORSO_IMU_PLOT
    if (_plot_torsoImu)
    {
        // Linear Acceleration (indices 0-2)
        _plot_torsoImu->AddData(0, curTime, imu.linear_acceleration().a1());
        _plot_torsoImu->AddData(1, curTime, imu.linear_acceleration().a2());
        _plot_torsoImu->AddData(2, curTime, imu.linear_acceleration().a3());
        
        // Angular Velocity (indices 3-5)
        _plot_torsoImu->AddData(3, curTime, imu.angular_velocity().a1());
        _plot_torsoImu->AddData(4, curTime, imu.angular_velocity().a2());
        _plot_torsoImu->AddData(5, curTime, imu.angular_velocity().a3());
        
        // Quaternion Orientation (indices 6-9)
        _plot_torsoImu->AddData(6, curTime, imu.orientation().w());
        _plot_torsoImu->AddData(7, curTime, imu.orientation().x());
        _plot_torsoImu->AddData(8, curTime, imu.orientation().y());
        _plot_torsoImu->AddData(9, curTime, imu.orientation().z());
        
        _plot_torsoImu->DataUpdated(curTime);
    }
#endif
}

void G1DataHandler::OnRecvArbitraryState(const double curTime, const dtproto::std_msgs::PackedDouble &state)
{
#ifdef ENABLE_DEBUG_DATA_PLOT
    if (_plot_debugData)
    {
        int dataSize = state.data_size();
        if (_debug_data_array_size != dataSize)
        {
            _debug_data_array_size = dataSize;
            _plot_debugData->ResetPlot();
            for (int i = 0; i < std::min(dataSize, _debug_data_array_size_max); i++)
            {
                _plot_debugData->AddGraph(QString("data[%1]").arg(i), LineColor(i));
            }
        }

        for (int i = 0; i < std::min(dataSize, _debug_data_array_size_max); i++)
        {
            _plot_debugData->AddData(i, curTime, state.data(i));
        }
    }
#endif
}

void G1DataHandler::OnRecvArbitraryStateXY(const double curTime, const dtproto::std_msgs::PackedDouble &state)
{
#ifdef ENABLE_DEBUG_DATA_XY_PLOT
    if (_plot_debugDataXY)
    {
        int dataSize = state.data_size();
        if (_debug_data_xy_array_size != dataSize)
        {
            _debug_data_xy_array_size = dataSize;
            _plot_debugDataXY->ResetPlot();
            for (int i = 0; i < std::min(dataSize / 2, _debug_data_xy_array_size_max / 2); i++)
            {
                _plot_debugDataXY->AddGraph(QString("data[%1]").arg(i), LineColor(i));
            }
        }

        for (int i = 0; i < std::min(dataSize / 2, _debug_data_xy_array_size_max / 2); i++)
        {
            double x = state.data(i * 2);
            double y = state.data(i * 2 + 1);
            _plot_debugDataXY->AddData(i, x, y);
        }
    }
#endif
}

void G1DataHandler::OnLoadTriggered(QString filename)
{
    // Implement MCAP file loading if needed
    // qDebug() << "G1DataHandler: Load triggered for file:" << filename;
}

void G1DataHandler::OnClearTriggered()
{
    ClearPlotData();
    _data_seq = 0;
    // qDebug() << "G1DataHandler: Clear triggered.";
}
