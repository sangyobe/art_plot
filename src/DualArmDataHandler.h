#ifndef DUALARMATAHANDLER_H
#define DUALARMATAHANDLER_H

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

#include "DualArm.pb.h"

class DualArmDataHandler : public DataHandler
{
    Q_OBJECT

private:
    MainWindow *_plotToolbox;

public:
    DualArmDataHandler(MainWindow *plotToolbox);
    ~DualArmDataHandler();

private:
    void BuildPlots();

    void OnRecvDualArmStateTimeStamped(const char *topic_name,
                                       const dtproto::dualarm::DualArmStateTimeStamped &state,
                                       const long long time, const long long clock);

    void OnRecvDualArmState(const double curTime, const dtproto::dualarm::DualArmState &state);
    void OnRecvControlState(const double curTime, const dtproto::dualarm::ControlState &actState, const dtproto::dualarm::ControlState &desState);
    void OnRecvJointState(const double curTime,
                          const google::protobuf::RepeatedPtrField<dtproto::dualarm::JointState> &state,
                          const google::protobuf::RepeatedPtrField<dtproto::dualarm::JointData> &actData,
                          const google::protobuf::RepeatedPtrField<dtproto::dualarm::JointData> &desData);
    void OnRecvTaskState(const double curTime,
                         const google::protobuf::RepeatedPtrField<dtproto::dualarm::TaskState> &actState,
                         const google::protobuf::RepeatedPtrField<dtproto::dualarm::TaskState> &desState);
    void OnRecvThreadState(const double curTime, const dtproto::dualarm::ThreadState &state);
    void OnRecvArbitraryState(const double curTime, const dtproto::std_msgs::PackedDouble &state);

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

    // for plotting user-defined arbitrary data array
    std::unique_ptr<PlotWindow> _plot_debugData{nullptr};
    int _debug_data_array_size{-1};
    int _debug_data_array_size_max{0};

#ifdef USE_TRANSPORT_ECAL
    std::unique_ptr<eCAL::protobuf::CSubscriber<dtproto::dualarm::DualArmStateTimeStamped>> _sub_state;
#endif
#ifdef USE_TRANSPORT_GRPC
    std::unique_ptr<dt::DAQ::StateSubscriberGrpc<dtproto::dualarm::DualArmStateTimeStamped>> _sub_state;
    std::thread _sub_reconnector;
    std::atomic<bool> _sub_reconnector_running;
#endif

    long long _data_seq{0}; // data sequence number in the plot
    uint32_t _msg_seq{0};   // message sequence number in the received message header

    // Qt specific
private slots:
    void OnLoadTriggered(QString filename) override;
    void OnClearTriggered() override;
};

#endif // DUALARMATAHANDLER_H