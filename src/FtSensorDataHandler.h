#ifndef FTSENSORDATAHANDLER_H
#define FTSENSORDATAHANDLER_H

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

#include <dtProto/sensor_msgs/FtSensor.pb.h>

class FtSensorDataHandler : public DataHandler
{
    Q_OBJECT

private:
    MainWindow *_plotToolbox;

public:
    FtSensorDataHandler(MainWindow *plotToolbox);
    ~FtSensorDataHandler();

private:
    void BuildPlots();

    void OnRecvFtTimeStamped(const char *topic_name,
                                       const dtproto::sensor_msgs::FtTimeStamped &msg,
                                       const long long time, const long long clock);

    void OnRecvFt(const double curTime, const dtproto::sensor_msgs::Ft &ft);

private:
    std::unique_ptr<PlotWindow> _plot_ft{nullptr};

#ifdef USE_TRANSPORT_ECAL
    std::unique_ptr<eCAL::protobuf::CSubscriber<dtproto::sensor_msgs::FtTimeStamped>> _sub_ft;
#endif
#ifdef USE_TRANSPORT_GRPC
    std::unique_ptr<dt::DAQ::StateSubscriberGrpc<dtproto::sensor_msgs::FtTimeStamped>> _sub_ft;
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

#endif // FTSENSORDATAHANDLER_H