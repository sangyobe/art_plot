#ifndef MOTORAGINGDATAHANDLER_H
#define MOTORAGINGDATAHANDLER_H

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

#include "proto/MotorAging.pb.h"

class MotorAgingDataHandler : public DataHandler
{
    Q_OBJECT

private:
    MainWindow *_plotToolbox;

public:
    MotorAgingDataHandler(MainWindow *plotToolbox);
    ~MotorAgingDataHandler();

private:
    void BuildPlots();

    void OnRecvMotorAgingStateTimeStamped(const char *topic_name,
                                          const dtproto::motor_aging::MotorAgingStateTimeStamped &state,
                                          const long long time, const long long clock);

    void OnRecvMotorAgingState(const double curTime, const dtproto::motor_aging::MotorAgingState &state);
    void OnRecvJointState(const double curTime,
                          const google::protobuf::RepeatedPtrField<dtproto::motor_aging::JointState> &state,
                          const google::protobuf::RepeatedPtrField<dtproto::motor_aging::JointData> &actData,
                          const google::protobuf::RepeatedPtrField<dtproto::motor_aging::JointData> &desData);
    void OnRecvThreadState(const double curTime, const dtproto::motor_aging::ThreadState &state);

private:
    std::unique_ptr<PlotWindow> _plot_threadState{nullptr};
    std::unique_ptr<PlotWindow> _plot_jointPos{nullptr};
    std::unique_ptr<PlotWindow> _plot_jointVel{nullptr};
    std::unique_ptr<PlotWindow> _plot_jointAcc{nullptr};
    std::unique_ptr<PlotWindow> _plot_jointTau{nullptr};
    std::unique_ptr<PlotWindow> _plot_incEnc{nullptr};
    std::unique_ptr<PlotWindow> _plot_absEnc{nullptr};

#ifdef USE_TRANSPORT_ECAL
    std::unique_ptr<eCAL::protobuf::CSubscriber<dtproto::motor_aging::MotorAgingStateTimeStamped>> _sub_state;
#endif
#ifdef USE_TRANSPORT_GRPC
    std::unique_ptr<dt::DAQ::StateSubscriberGrpc<dtproto::motor_aging::MotorAgingStateTimeStamped>> _sub_state;
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

#endif // MOTORAGINGDATAHANDLER_H