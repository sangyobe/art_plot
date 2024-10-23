#ifndef WOLYDATAHANDLER_H
#define WOLYDATAHANDLER_H

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

#include "proto/Woly.pb.h"

class WolyDataHandler : public DataHandler {
private:
    MainWindow* _plotToolbox;
public:
    WolyDataHandler(MainWindow* plotToolbox);
    ~WolyDataHandler();

private:
    void BuildPlots();

    void OnRecvWolyStateTimeStamped(const char *topic_name,
                                      const dtproto::woly::WolyStateTimeStamped &state,
                                      const long long time, const long long clock);

    void OnRecvWolyState(const double curTime, const dtproto::woly::WolyState &state);
    void OnRecvControlStateActual(const double curTime, const dtproto::woly::ControlState &state);
    void OnRecvControlStateDesired(const double curTime, const dtproto::woly::ControlState &state);
    void OnRecvJointState(const double curTime, const dtproto::woly::JointData &state);

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
    std::unique_ptr<eCAL::protobuf::CSubscriber<dtproto::woly::WolyStateTimeStamped>> _sub_state;
#endif
#ifdef USE_TRANSPORT_GRPC
    std::unique_ptr<dt::DAQ::StateSubscriberGrpc<dtproto::woly::WolyStateTimeStamped>> _sub_state;
    std::thread _sub_reconnector;
    std::atomic<bool> _sub_reconnector_running;
#endif

    long long _data_seq{0}; // data sequence number in the plot
};

#endif // WOLYDATAHANDLER_H