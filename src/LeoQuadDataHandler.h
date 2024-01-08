#ifndef LEOQUADDATAHANDLER_H
#define LEOQUADDATAHANDLER_H

#include "define.h"
#include "datahandler.h"

#ifdef USE_TRANSPORT_ECAL
#include <ecal/ecal.h>
#include <ecal/msg/protobuf/subscriber.h>
#endif

#ifdef USE_TRANSPORT_GRPC
#include <dtProto/Service.grpc.pb.h>
#include <dtCore/src/dtDAQ/grpc/dtStateSubscriberGrpc.hpp>
#endif

#include "LeoQuad.pb.h"

class LeoQuadDataHandler : public DataHandler {
private:
    MainWindow* _plotToolbox;
public:
    LeoQuadDataHandler(MainWindow* plotToolbox);
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

private:
    std::unique_ptr<PlotWindow> _plot_threadState;
    std::unique_ptr<PlotWindow> _plot_comPos;
    std::unique_ptr<PlotWindow> _plot_comVel;
    std::unique_ptr<PlotWindow> _plot_orient;
    std::unique_ptr<PlotWindow> _plot_angVel;
    std::unique_ptr<PlotWindow> _plot_footPos;
    std::unique_ptr<PlotWindow> _plot_footForce;
    std::unique_ptr<PlotWindow> _plot_contact;
    std::unique_ptr<PlotWindow> _plot_cpgCpg;
    std::unique_ptr<PlotWindow> _plot_cpgPhi;
    std::unique_ptr<PlotWindow> _plot_jointPos;
    std::unique_ptr<PlotWindow> _plot_jointVel;
    std::unique_ptr<PlotWindow> _plot_jointAcc;
    std::unique_ptr<PlotWindow> _plot_jointTau;
    std::unique_ptr<PlotWindow> _plot_incEnc;
    std::unique_ptr<PlotWindow> _plot_absEnc;

#ifdef USE_TRANSPORT_ECAL
    std::unique_ptr<eCAL::protobuf::CSubscriber<dtproto::leoquad::LeoQuadStateTimeStamped>> _sub_state;
#endif
#ifdef USE_TRANSPORT_GRPC
    std::unique_ptr<dtCore::dtStateSubscriberGrpc<dtproto::leoquad::LeoQuadStateTimeStamped>> _sub_state;
    std::thread _sub_reconnector;
    std::atomic<bool> _sub_reconnector_running;
#endif
};

#endif // LEOQUADDATAHANDLER_H