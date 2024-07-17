#ifndef QUADIPDATAHANDLER_H
#define QUADIPDATAHANDLER_H

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

#include "QuadIP.pb.h"

class QuadIPDataHandler : public DataHandler {
private:
    MainWindow* _plotToolbox;
public:
    QuadIPDataHandler(MainWindow* plotToolbox);
    ~QuadIPDataHandler();

private:
    void BuildPlots();

    void OnRecvQuadIpStateTimeStamped(const char *topic_name,
                                      const dtproto::quadip::QuadIpStateTimeStamped &state,
                                      const long long time, const long long clock);

    void OnRecvQuadIpState(const double curTime, const dtproto::quadip::QuadIpState &state);
    void OnRecvControlStateActual(const double curTime, const dtproto::quadip::ControlState &state);
    void OnRecvControlStateDesired(const double curTime, const dtproto::quadip::ControlState &state);
    void OnRecvCpgState(const double curTime, const dtproto::quadip::CpgState &state);
    void OnRecvJointState(const double curTime, const dtproto::quadip::JointState &state);

private:
    std::unique_ptr<PlotWindow> _plot_comPos;
    std::unique_ptr<PlotWindow> _plot_comVel;
    std::unique_ptr<PlotWindow> _plot_orient;
    std::unique_ptr<PlotWindow> _plot_angVel;
    std::unique_ptr<PlotWindow> _plot_footPos;
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
    std::unique_ptr<eCAL::protobuf::CSubscriber<dtproto::quadip::QuadIpStateTimeStamped>> _sub_state;
#endif
#ifdef USE_TRANSPORT_GRPC
    std::unique_ptr<dt::DAQ::StateSubscriberGrpc<dtproto::quadip::QuadIpStateTimeStamped>> _sub_state;
    std::thread _sub_reconnector;
    std::atomic<bool> _sub_reconnector_running;
#endif
};

#endif // QUADIPDATAHANDLER_H