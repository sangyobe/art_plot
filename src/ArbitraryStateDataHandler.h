#ifndef ARBITRARYSTATEDATAHANDLER_H
#define ARBITRARYSTATEDATAHANDLER_H

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

#include <dtProto/robot_msgs/ArbitraryState.pb.h>

class ArbitraryStateDataHandler : public DataHandler {
private:
    MainWindow* _plotToolbox;
public:
    ArbitraryStateDataHandler(MainWindow* plotToolbox);
    ~ArbitraryStateDataHandler();

private:
    void BuildPlots();

    void OnRecvArbitraryStateTimeStamped(const char *topic_name,
                                      const dtproto::robot_msgs::ArbitraryStateTimeStamped &state,
                                      const long long time, const long long clock);

    void OnRecvArbitraryState(const double curTime, const dtproto::std_msgs::PackedDouble &state);

private:
    std::unique_ptr<PlotWindow> _plot;
    int _data_size{-1};
    const int _data_size_max = 16;

#ifdef USE_TRANSPORT_ECAL
    std::unique_ptr<eCAL::protobuf::CSubscriber<dtproto::robot_msgs::ArbitraryStateTimeStamped>> _sub_state;
#endif
#ifdef USE_TRANSPORT_GRPC
    std::unique_ptr<dt::DAQ::StateSubscriberGrpc<dtproto::robot_msgs::ArbitraryStateTimeStamped>> _sub_state;
    std::thread _sub_reconnector;
    std::atomic<bool> _sub_reconnector_running;
#endif

    long long _data_seq{0}; // data sequence number in the plot
};

#endif // ARBITRARYSTATEDATAHANDLER_H