#ifndef LEOQUADDATAHANDLER_H
#define LEOQUADDATAHANDLER_H

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

#include "LeoQuad.pb.h"

class LeoQuadDataHandler : public DataHandler
{
    Q_OBJECT

private:
    MainWindow *_plotToolbox;

public:
    LeoQuadDataHandler(MainWindow *plotToolbox);
    ~LeoQuadDataHandler();

private:
    void BuildPlots();

    void OnRecvLeoQuadStateTimeStamped(const char *topic_name,
                                       const dtproto::leoquad::LeoQuadStateTimeStamped &state,
                                       const long long time, const long long clock);

    void OnRecvLeoQuadState(const double curTime, const dtproto::leoquad::LeoQuadState &state);
    void OnRecvCpgState(const double curTime, const dtproto::leoquad::CpgState &state);
    void OnRecvControlState(const double curTime, const dtproto::leoquad::ControlState &actState, const dtproto::leoquad::ControlState &desState);
    void OnRecvJointState(const double curTime,
                          const google::protobuf::RepeatedPtrField<dtproto::leoquad::JointState> &state,
                          const google::protobuf::RepeatedPtrField<dtproto::leoquad::JointData> &actData,
                          const google::protobuf::RepeatedPtrField<dtproto::leoquad::JointData> &desData);
    void OnRecvThreadState(const double curTime, const dtproto::leoquad::ThreadState &state);
    void OnRecvArbitraryState(const double curTime, const dtproto::std_msgs::PackedDouble &state);

private:
    std::unique_ptr<PlotWindow> _plot_threadState{nullptr};
    std::unique_ptr<PlotWindow> _plot_comPos{nullptr};
    std::unique_ptr<PlotWindow> _plot_comVel{nullptr};
    std::unique_ptr<PlotWindow> _plot_orient{nullptr};
    std::unique_ptr<PlotWindow> _plot_angVel{nullptr};
    std::unique_ptr<PlotWindow> _plot_footPos{nullptr};
    std::unique_ptr<PlotWindow> _plot_footVel{nullptr};
    std::unique_ptr<PlotWindow> _plot_footForce{nullptr};
    std::unique_ptr<PlotWindow> _plot_contact{nullptr};
    std::unique_ptr<PlotWindow> _plot_cpgCpg{nullptr};
    std::unique_ptr<PlotWindow> _plot_cpgPhi{nullptr};
    std::unique_ptr<PlotWindow> _plot_jointPos{nullptr};
    std::unique_ptr<PlotWindow> _plot_jointVel{nullptr};
    std::unique_ptr<PlotWindow> _plot_jointAcc{nullptr};
    std::unique_ptr<PlotWindow> _plot_jointTau{nullptr};
    std::unique_ptr<PlotWindow> _plot_incEnc{nullptr};
    std::unique_ptr<PlotWindow> _plot_absEnc{nullptr};
    std::unique_ptr<PlotWindow> _plot_eePosG_G{nullptr};
    std::unique_ptr<PlotWindow> _plot_eeVelE_G{nullptr};

    // for plotting user-defined arbitrary data array
    std::unique_ptr<PlotWindow> _plot_debugData{nullptr};
    int _debug_data_array_size{-1};
    int _debug_data_array_size_max{0};

#ifdef USE_TRANSPORT_ECAL
    std::unique_ptr<eCAL::protobuf::CSubscriber<dtproto::leoquad::LeoQuadStateTimeStamped>> _sub_state;
#endif
#ifdef USE_TRANSPORT_GRPC
    std::unique_ptr<dt::DAQ::StateSubscriberGrpc<dtproto::leoquad::LeoQuadStateTimeStamped>> _sub_state;
    std::thread _sub_reconnector;
    std::atomic<bool> _sub_reconnector_running;
#endif

    long long _msg_seq{0};

    // Qt specific
private slots:
    void OnLoadTriggered(QString filename);
};

#endif // LEOQUADDATAHANDLER_H