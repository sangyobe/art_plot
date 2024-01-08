#ifndef ANONDATAARRAYHANDLER_H
#define ANONDATAARRAYHANDLER_H

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

#include <dtProto/robot_msgs/AnonState.pb.h>

class AnonDataArrayHandler : public DataHandler {
private:
    MainWindow* _plotToolbox;
public:
    AnonDataArrayHandler(MainWindow* plotToolbox);
    ~AnonDataArrayHandler();

private:
    void BuildPlots();

    void OnRecvAnonStateTimeStamped(const char *topic_name,
                                      const dtproto::robot_msgs::AnonStateTimeStamped &state,
                                      const long long time, const long long clock);

    void OnRecvAnonState(const double curTime, const dtproto::std_msgs::PackedDouble &state);

private:
    std::unique_ptr<PlotWindow> _plot;
    int _data_size{-1};
    const int _data_size_max = 16;

#ifdef USE_TRANSPORT_ECAL
    std::unique_ptr<eCAL::protobuf::CSubscriber<dtproto::robot_msgs::AnonStateTimeStamped>> _sub_state;
#endif
#ifdef USE_TRANSPORT_GRPC
    std::unique_ptr<dtCore::dtStateSubscriberGrpc<dtproto::robot_msgs::AnonStateTimeStamped>> _sub_state;
    std::thread _sub_reconnector;
    std::atomic<bool> _sub_reconnector_running;
#endif
};

#endif // ANONDATAARRAYHANDLER_H