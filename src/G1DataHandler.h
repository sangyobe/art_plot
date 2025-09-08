#ifndef G1DATAHANDLER_H
#define G1DATAHANDLER_H

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

#include "proto/G1.pb.h"

class G1DataHandler : public DataHandler
{
    Q_OBJECT

private:
    MainWindow *_plotToolbox;

public:
    G1DataHandler(MainWindow *plotToolbox);
    ~G1DataHandler();

private:
    void BuildPlots();

    void OnRecvG1StateTimeStamped(const char *topic_name,
                                  const dtproto::g1::G1StateTimeStamped &state,
                                  const long long time, const long long clock);

    void OnRecvG1State(const double curTime, const dtproto::g1::G1State &state);
    void OnRecvJointState(const double curTime,
                          const google::protobuf::RepeatedPtrField<dtproto::g1::JointData> &actData,
                          const google::protobuf::RepeatedPtrField<dtproto::g1::JointData> &desData);
    void OnRecvThreadState(const double curTime, const dtproto::g1::ThreadState &state);
    void OnRecvArbitraryState(const double curTime, const dtproto::std_msgs::PackedDouble &state);
    void OnRecvArbitraryStateXY(const double curTime, const dtproto::std_msgs::PackedDouble &state);
    void OnRecvPelvisImu(const double curTime, const dtproto::sensor_msgs::Imu &imu);
    void OnRecvTorsoImu(const double curTime, const dtproto::sensor_msgs::Imu &imu);

private:
    // Plot windows for different data types
    std::unique_ptr<PlotWindow> _plot_threadState{nullptr};
    // Joint Position plots (separated)
    std::unique_ptr<PlotWindow> _plot_upperJointPos{nullptr};
    std::unique_ptr<PlotWindow> _plot_lowerJointPos{nullptr};
    // Joint Velocity plots (separated)
    std::unique_ptr<PlotWindow> _plot_upperJointVel{nullptr};
    std::unique_ptr<PlotWindow> _plot_lowerJointVel{nullptr};
    // Joint Acceleration plots (separated)
    std::unique_ptr<PlotWindow> _plot_upperJointAcc{nullptr};
    std::unique_ptr<PlotWindow> _plot_lowerJointAcc{nullptr};
    // Joint Torque plots (separated)
    std::unique_ptr<PlotWindow> _plot_upperJointTau{nullptr};
    std::unique_ptr<PlotWindow> _plot_lowerJointTau{nullptr};
    // IMU plots
    std::unique_ptr<PlotWindow> _plot_pelvisImu{nullptr};
    std::unique_ptr<PlotWindow> _plot_torsoImu{nullptr};

    // for plotting user-defined debug data array
    // std::unique_ptr<PlotWindow> _plot_debugData{nullptr};
    // int _debug_data_array_size{-1};
    // int _debug_data_array_size_max{0};

    // for plotting user-defined debug data array(2D)
    // std::unique_ptr<PlotWindow> _plot_debugDataXY{nullptr};
    // int _debug_data_xy_array_size{-1};
    // int _debug_data_xy_array_size_max{0};

#ifdef USE_TRANSPORT_ECAL
    std::unique_ptr<eCAL::protobuf::CSubscriber<dtproto::g1::G1StateTimeStamped>> _sub_state;
#endif
#ifdef USE_TRANSPORT_GRPC
    std::unique_ptr<dt::DAQ::StateSubscriberGrpc<dtproto::g1::G1StateTimeStamped>> _sub_state;
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

#endif // G1DATAHANDLER_H
