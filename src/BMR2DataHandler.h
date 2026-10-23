#ifndef BMR2DATAHANDLER_H
#define BMR2DATAHANDLER_H

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

#include "proto/BMR2.pb.h"

class BMR2DataHandler : public DataHandler
{
    Q_OBJECT

private:
    MainWindow *_plotToolbox;

public:
    BMR2DataHandler(MainWindow *plotToolbox);
    ~BMR2DataHandler();

private:
    void BuildPlots();

    void OnRecvBMR2StateTimeStamped(const char *topic_name,
                                       const dtproto::bmr2::BMR2StateTimeStamped &state,
                                       const long long time, const long long clock);

    void OnRecvBMR2State(const double curTime, const dtproto::bmr2::BMR2State &state);
    void OnRecvControlState(const double curTime, const dtproto::bmr2::ControlState &actState, const dtproto::bmr2::ControlState &desState);
    void OnRecvJointState(const double curTime,
                          const google::protobuf::RepeatedPtrField<dtproto::bmr2::JointState> &state,
                          const google::protobuf::RepeatedPtrField<dtproto::bmr2::JointData> &actData,
                          const google::protobuf::RepeatedPtrField<dtproto::bmr2::JointData> &desData);
    void OnRecvTaskState(const double curTime,
                         const google::protobuf::RepeatedPtrField<dtproto::bmr2::TaskState> &actState,
                         const google::protobuf::RepeatedPtrField<dtproto::bmr2::TaskState> &desState);
    void OnRecvThreadState(const double curTime, const dtproto::bmr2::ThreadState &state);
    void OnRecvArbitraryState(const double curTime, const dtproto::std_msgs::PackedDouble &state);
    void OnRecvArbitraryStateXY(const double curTime, const dtproto::std_msgs::PackedDouble &state);

private:
    std::unique_ptr<PlotWindow> _plot_threadState{nullptr};
    std::unique_ptr<PlotWindow> _plot_jointPos{nullptr};
    std::unique_ptr<PlotWindow> _plot_jointVel{nullptr};
    std::unique_ptr<PlotWindow> _plot_jointAcc{nullptr};
    std::unique_ptr<PlotWindow> _plot_jointTau{nullptr};
    std::unique_ptr<PlotWindow> _plot_taskPos{nullptr};
    std::unique_ptr<PlotWindow> _plot_taskVel{nullptr};
    std::unique_ptr<PlotWindow> _plot_taskTau{nullptr};
    std::unique_ptr<PlotWindow> _plot_incEnc{nullptr};
    std::unique_ptr<PlotWindow> _plot_absEnc{nullptr};

    // for plotting user-defined debug data array
    std::unique_ptr<PlotWindow> _plot_debugData{nullptr};
    int _debug_data_array_size{-1};
    int _debug_data_array_size_max{0};

    // for plotting user-defined debug data array(2D)
    std::unique_ptr<PlotWindow> _plot_debugDataXY{nullptr};
    int _debug_data_xy_array_size{-1};
    int _debug_data_xy_array_size_max{0};

#ifdef USE_TRANSPORT_ECAL
    std::unique_ptr<eCAL::protobuf::CSubscriber<dtproto::bmr2::BMR2StateTimeStamped>> _sub_state;
#endif
#ifdef USE_TRANSPORT_GRPC
    std::unique_ptr<dt::DAQ::StateSubscriberGrpc<dtproto::bmr2::BMR2StateTimeStamped>> _sub_state;
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

#endif // BMR2DATAHANDLER_H