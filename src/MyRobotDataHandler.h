#ifndef MYROBOTDATAHANDLER_H
#define MYROBOTDATAHANDLER_H

#include "config.h"
#include "datahandler.h"

#ifdef USE_TRANSPORT_ECAL
#include <ecal/ecal.h>
#include <ecal/msg/protobuf/subscriber.h>
#endif

#ifdef USE_TRANSPORT_GRPC
#include <dtCore/src/dtDAQ/grpc/dtStateSubscriberGrpc.hpp>
#include <dtProto/Service.grpc.pb.h>
#endif

#include "proto/MyRobot.pb.h"

class MyRobotDataHandler : public DataHandler
{
    Q_OBJECT

private:
    MainWindow *_plotToolbox;

public:
    MyRobotDataHandler(MainWindow *plotToolbox);
    ~MyRobotDataHandler();

private:
    void BuildPlots();

    void OnRecvMyRobotStateTimeStamped(const char *topic_name,
                                       const dtproto::myrobot::MyRobotStateTimeStamped &state,
                                       const long long time, const long long clock);

    void OnRecvMyRobotState(const double curTime, const dtproto::myrobot::MyRobotState &state);
    void OnRecvJointState(const double curTime,
                          const google::protobuf::RepeatedPtrField<dtproto::myrobot::JointState> &state,
                          const google::protobuf::RepeatedPtrField<dtproto::myrobot::JointData> &actData,
                          const google::protobuf::RepeatedPtrField<dtproto::myrobot::JointData> &desData);
    void OnRecvThreadState(const double curTime, const dtproto::myrobot::ThreadState &state);
    void OnRecvArbitraryState(const double curTime, const dtproto::std_msgs::PackedDouble &state);

private:
    std::unique_ptr<PlotWindow> _plot_threadState{nullptr};
    std::unique_ptr<PlotWindow> _plot_jointPos{nullptr};
    std::unique_ptr<PlotWindow> _plot_jointVel{nullptr};
    std::unique_ptr<PlotWindow> _plot_jointAcc{nullptr};
    std::unique_ptr<PlotWindow> _plot_jointTau{nullptr};

    // for plotting user-defined debug data array
    std::unique_ptr<PlotWindow> _plot_debugData{nullptr};
    int _debug_data_array_size{-1};
    int _debug_data_array_size_max{0};

#ifdef USE_TRANSPORT_ECAL
    std::unique_ptr<eCAL::protobuf::CSubscriber<dtproto::myrobot::MyRobotStateTimeStamped>> _sub_state;
#endif
#ifdef USE_TRANSPORT_GRPC
    std::unique_ptr<dt::DAQ::StateSubscriberGrpc<dtproto::myrobot::MyRobotStateTimeStamped>> _sub_state;
    std::thread _sub_reconnector;
    std::atomic<bool> _sub_reconnector_running;
#endif

    long long _data_seq{0}; // data sequence number in the plot
    double _data_seq_zero_time; // time at data sequence 0
    uint32_t _msg_seq{0};   // message sequence number in the received message header

    // Qt specific
private slots:
    void OnLoadTriggered(QString filename) override;
    void OnClearTriggered() override;
};

#endif // MYROBOTDATAHANDLER_H