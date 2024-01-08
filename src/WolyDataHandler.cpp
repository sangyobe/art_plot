#include "WolyDataHandler.h"
#include "mainwindow.h"
#include "pconstants.h"
#include "plotwindow.h"
#include "cmdopts.hpp"
#include "stringutil.hpp"
#include <QApplication>
#include <QDebug>
#include <dtCore/src/dtLog/dtLog.h>

//#define PRINT_PUB_SUB_INFO

// #define ENABLE_COM_POS_PLOT
// #define ENABLE_COM_VEL_PLOT
// #define ENABLE_ORIENTATION_PLOT
// #define ENABLE_ANGULAR_VEL_PLOT
// #define ENABLE_FOOT_CONTACT_PLOT
// #define ENABLE_CPG_PHI_PLOT
// #define ENABLE_CPG_CPG_PLOT
#define ENABLE_JOINT_POSISION_PLOT
// #define ENABLE_JOINT_VELOCITY_PLOT
// #define ENABLE_JOINT_ACCELERATION_PLOT
// #define ENABLE_JOINT_TORQUE_PLOT
// #define ENABLE_JOINT_ABSOLUTE_ENCODER_PLOT
// #define ENABLE_JOINT_INCREMENTAL_ENCODER_PLOT
// #define ENABLE_FOOT_TRAJECTORY_PLOT

constexpr static int jdof = 16;
constexpr static int legnum = 4;

WolyDataHandler::WolyDataHandler(MainWindow* plotToolbox) 
: DataHandler(plotToolbox)
#ifdef ENABLE_COM_POS_PLOT
, _plot_comPos(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_COM_VEL_PLOT
, _plot_comVel(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_ORIENTATION_PLOT
, _plot_orient(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_ANGULAR_VEL_PLOT
, _plot_angVel(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_FOOT_TRAJECTORY_PLOT
, _plot_footPos(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_FOOT_CONTACT_PLOT
, _plot_contact(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_FOOT_FORCE_PLOT
, _plot_footForce(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_CPG_CPG_PLOT
, _plot_cpgCpg(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_CPG_PHI_PLOT
, _plot_cpgPhi(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_JOINT_POSISION_PLOT
, _plot_jointPos(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_JOINT_VELOCITY_PLOT
, _plot_jointVel(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_JOINT_ACCELERATION_PLOT
, _plot_jointAcc(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_JOINT_TORQUE_PLOT
, _plot_jointTau(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_JOINT_INCREMENTAL_ENCODER_PLOT
, _plot_incEnc(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_JOINT_ABSOLUTE_ENCODER_PLOT
, _plot_absEnc(std::make_unique<PlotWindow>(plotToolbox))
#endif
{
    BuildPlots();
}

WolyDataHandler::~WolyDataHandler()
{
#ifdef USE_TRANSPORT_GRPC
    _sub_reconnector_running = false;
    if (_sub_reconnector.joinable())
        _sub_reconnector.join();
#endif
}

void WolyDataHandler::BuildPlots()
{
#ifdef ENABLE_COM_POS_PLOT
    // _plot_comPos = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_comPos->SetWindowTitle("COM position wrt World");
    _plot_comPos->AddGraph("Com.x.desired", LineColor<0>());
    _plot_comPos->AddGraph("Com.y.desired", LineColor<1>());
    _plot_comPos->AddGraph("Com.z.desired", LineColor<2>());
    _plot_comPos->AddGraph("Com.x.actual", LineColor<3>());
    _plot_comPos->AddGraph("Com.y.actual", LineColor<4>());
    _plot_comPos->AddGraph("Com.z.actual", LineColor<5>());
    _plot_comPos->show();
    RegisterPlot(_plot_comPos.get());
#endif

#ifdef ENABLE_COM_VEL_PLOT
    // _plot_comVel = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_comVel->SetWindowTitle("COM velocity wrt World");
    _plot_comVel->AddGraph("Com.Vx.desired", LineColor<0>());
    _plot_comVel->AddGraph("Com.Vy.desired", LineColor<1>());
    _plot_comVel->AddGraph("Com.Vz.desired", LineColor<2>());
    _plot_comVel->AddGraph("Com.Vx.actual", LineColor<3>());
    _plot_comVel->AddGraph("Com.Vy.actual", LineColor<4>());
    _plot_comVel->AddGraph("Com.Vz.actual", LineColor<5>());
    _plot_comVel->show();
    RegisterPlot(_plot_comVel.get());
#endif

#ifdef ENABLE_ORIENTATION_PLOT
    // _plot_orient = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_orient->SetWindowTitle("Body orientation wrt World");
    _plot_orient->AddGraph("Euler.x.desired", LineColor<6>());
    _plot_orient->AddGraph("Euler.y.desired", LineColor<7>());
    _plot_orient->AddGraph("Euler.z.desired", LineColor<8>());
    _plot_orient->AddGraph("Euler.x.actual", LineColor<9>());
    _plot_orient->AddGraph("Euler.y.actual", LineColor<10>());
    _plot_orient->AddGraph("Euler.z.actual", LineColor<11>());
    _plot_orient->show();
    RegisterPlot(_plot_orient.get());
    plotDataHandler.SetPlotOrient(_plot_orient.get());
#endif

#ifdef ENABLE_ANGULAR_VEL_PLOT
    // _plot_angVel = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_angVel->SetWindowTitle("Body angular velocity wrt World");
    _plot_angVel->AddGraph("AngularVel.x.desired", LineColor<6>());
    _plot_angVel->AddGraph("AngularVel.y.desired", LineColor<7>());
    _plot_angVel->AddGraph("AngularVel.z.desired", LineColor<8>());
    _plot_angVel->AddGraph("AngularVel.x.actual", LineColor<9>());
    _plot_angVel->AddGraph("AngularVel.y.actual", LineColor<10>());
    _plot_angVel->AddGraph("AngularVel.z.actual", LineColor<11>());
    _plot_angVel->show();
    RegisterPlot(_plot_angVel.get());
#endif

#ifdef ENABLE_FOOT_TRAJECTORY_PLOT
    // _plot_footPos = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_footPos->SetWindowTitle("Foot trajectory wrt Body");
    _plot_footPos->AddGraph("FL.x.desired", LineColor<0>());
    _plot_footPos->AddGraph("FL.y.desired", LineColor<1>());
    _plot_footPos->AddGraph("FL.z.desired", LineColor<2>());
    _plot_footPos->AddGraph("FL.x.actual", LineColor<3>());
    _plot_footPos->AddGraph("FL.y.actual", LineColor<4>());
    _plot_footPos->AddGraph("FL.z.actual", LineColor<5>());
    _plot_footPos->AddGraph("BL.x.desired", LineColor<6>());
    _plot_footPos->AddGraph("BL.y.desired", LineColor<7>());
    _plot_footPos->AddGraph("BL.z.desired", LineColor<8>());
    _plot_footPos->AddGraph("BL.x.actual", LineColor<9>());
    _plot_footPos->AddGraph("BL.y.actual", LineColor<10>());
    _plot_footPos->AddGraph("BL.z.actual", LineColor<11>());
    _plot_footPos->AddGraph("FR.x.desired", LineColor<12>());
    _plot_footPos->AddGraph("FR.y.desired", LineColor<13>());
    _plot_footPos->AddGraph("FR.z.desired", LineColor<14>());
    _plot_footPos->AddGraph("FR.x.actual", LineColor<15>());
    _plot_footPos->AddGraph("FR.y.actual", LineColor<16>());
    _plot_footPos->AddGraph("FR.z.actual", LineColor<17>());
    _plot_footPos->AddGraph("BR.x.desired", LineColor<18>());
    _plot_footPos->AddGraph("BR.y.desired", LineColor<19>());
    _plot_footPos->AddGraph("BR.z.desired", LineColor<20>());
    _plot_footPos->AddGraph("BR.x.actual", LineColor<21>());
    _plot_footPos->AddGraph("BR.y.actual", LineColor<22>());
    _plot_footPos->AddGraph("BR.z.actual", LineColor<23>());
    _plot_footPos->show();
    RegisterPlot(_plot_footPos.get());
#endif

#ifdef ENABLE_FOOT_CONTACT_PLOT
    // _plot_contact = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_contact->SetWindowTitle("Foot-floor contact state");
    _plot_contact->AddGraph("FL", LineColor<12>());
    _plot_contact->AddGraph("BL", LineColor<13>());
    _plot_contact->AddGraph("FR", LineColor<14>());
    _plot_contact->AddGraph("BR", LineColor<15>());
    _plot_contact->show();
    RegisterPlot(_plot_contact.get());
#endif

#ifdef ENABLE_CPG_PHI_PLOT
    // _plot_phi = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_phi->SetWindowTitle("Cpg.phi");
    _plot_phi->AddGraph("Phi.a1", LineColor<10>());
    _plot_phi->AddGraph("Phi.a2", LineColor<11>());
    _plot_phi->AddGraph("Phi.a3", LineColor<12>());
    _plot_phi->AddGraph("Phi.a4", LineColor<13>());
    _plot_phi->show();
    RegisterPlot(_plot_phi.get());
#endif

#ifdef ENABLE_CPG_CPG_PLOT
    // _plot_cpg = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_cpg->SetWindowTitle("Cpg.cpg");
    _plot_cpg->AddGraph("Cpg.a1", LineColor<10>());
    _plot_cpg->AddGraph("Cpg.a2", LineColor<11>());
    _plot_cpg->AddGraph("Cpg.a3", LineColor<12>());
    _plot_cpg->AddGraph("Cpg.a4", LineColor<13>());
    _plot_cpg->show();
    RegisterPlot(_plot_cpg.get());
#endif

#ifdef ENABLE_JOINT_POSISION_PLOT
    //_plot_jointPos = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_jointPos->SetWindowTitle("Joint position");
    for (int ji = 0; ji < jdof; ji++) {
        _plot_jointPos->AddGraph(QString("Joint_%1.pos.desired").arg(ji+1, 2, 10, QLatin1Char('0')), LineColor(ji));
        _plot_jointPos->AddGraph(QString("Joint_%1.pos.actual").arg(ji+1, 2, 10, QLatin1Char('0')), LineColor(ji + jdof));
    }
    _plot_jointPos->show();
    RegisterPlot(_plot_jointPos.get());
#endif

#ifdef ENABLE_JOINT_VELOCITY_PLOT
    // _plot_jointVel = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_jointVel->SetWindowTitle("Joint velocity");
    for (int ji = 0; ji < jdof; ji++) {
        _plot_jointVel->AddGraph(QString("Joint_%1.vel.desired").arg(ji+1, 2, 10, QLatin1Char('0')), LineColor(ji));
        _plot_jointVel->AddGraph(QString("Joint_%1.vel.actual").arg(ji+1, 2, 10, QLatin1Char('0')), LineColor(ji + jdof));
    }
    _plot_jointVel->show();
    RegisterPlot(_plot_jointVel.get());
#endif

#ifdef ENABLE_JOINT_ACCELERATION_PLOT
    // _plot_jointAcc = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_jointAcc->SetWindowTitle("Joint acceleration");
    for (int ji = 0; ji < jdof; ji++) {
        _plot_jointAcc->AddGraph(QString("Joint_%1.acc.desired").arg(ji+1, 2, 10, QLatin1Char('0')), LineColor(ji));
        _plot_jointAcc->AddGraph(QString("Joint_%1.acc.actual").arg(ji+1, 2, 10, QLatin1Char('0')),
                                    LineColor(ji + jdof));
    }
    _plot_jointAcc->show();
    RegisterPlot(_plot_jointAcc.get());
    plotDataHandler.SetPlotJointAcc(_plot_jointAcc.get());
#endif

#ifdef ENABLE_JOINT_TORQUE_PLOT
    // _plot_jointTau = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_jointTau->SetWindowTitle("Joint torque");
    for (int ji = 0; ji < jdof; ji++) {
        _plot_jointTau->AddGraph(QString("Joint_%1.tau.desired").arg(ji+1, 2, 10, QLatin1Char('0')), LineColor(ji));
        _plot_jointTau->AddGraph(QString("Joint_%1.tau.actual").arg(ji+1, 2, 10, QLatin1Char('0')),
                                    LineColor(ji + jdof));
    }
    _plot_jointTau->show();
    RegisterPlot(_plot_jointTau.get());
#endif

#ifdef ENABLE_JOINT_ABSOLUTE_ENCODER_PLOT
    // _plot_absEnc = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_absEnc->SetWindowTitle("Joint absolute encoder");
    for (int ji = 0; ji < jdof; ji++) {
        _plot_absEnc->AddGraph(QString("Joint_%1.absolute_encoder").arg(ji+1, 2, 10, QLatin1Char('0')), LineColor(ji));
    }
    _plot_absEnc->show();
    RegisterPlot(_plot_absEnc.get());
#endif

#ifdef ENABLE_JOINT_INCREMENTAL_ENCODER_PLOT
    // _plot_incEnc = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_incEnc->SetWindowTitle("Joint incremental encoder");
    for (int ji = 0; ji < jdof; ji++) {
        _plot_incEnc->AddGraph(QString("Joint_%1.incremental_encoder").arg(ji+1, 2, 10, QLatin1Char('0')), LineColor(ji));
    }
    _plot_incEnc->show();
    RegisterPlot(_plot_incEnc.get());
#endif


    // 데이터 연결
#ifdef USE_TRANSPORT_ECAL
    _sub_state = std::make_unique<eCAL::protobuf::CSubscriber<dtproto::woly::WolyStateTimeStamped>>("RobotState");
    _sub_state->AddReceiveCallback(std::bind(
        &WolyDataHandler::OnRecvWolyStateTimeStamped, this,
        std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4));
#endif
#ifdef USE_TRANSPORT_GRPC
    std::string ip;
    uint16_t port;
    GetServerAddress(ip, port);
    if (ip.empty() || port == 0) {
        ip = "127.0.0.1";
        port = 50051;
    }
    std::string svr_address = string_format("%s:%d", ip.c_str(), port);
    
    _sub_state = std::make_unique<dtCore::dtStateSubscriberGrpc<dtproto::woly::WolyStateTimeStamped>>("RobotState", "svr_address");
    std::function<void(dtproto::woly::WolyStateTimeStamped&)> handler = [this](dtproto::woly::WolyStateTimeStamped& msg) {
        static long long seq = 0;
        this->OnRecvWolyStateTimeStamped("", msg, 0, seq++);
    };
    _sub_state->RegMessageHandler(handler);

    _sub_reconnector_running = true;
    _sub_reconnector = std::thread([this] {

        while (this->_sub_reconnector_running) {
            if (!this->_sub_state->IsRunning()) {
                this->_sub_state->Reconnect();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    });
#endif

}

void WolyDataHandler::OnRecvWolyStateTimeStamped(const char *topic_name,
                                    const dtproto::woly::WolyStateTimeStamped &state,
                                    const long long time, const long long clock)
{
    double curTime = clock * 1e-3;
    OnRecvWolyState(curTime, state.state());

#ifdef PRINT_PUB_SUB_INFO
    qDebug() << "------------------------------------------";
    qDebug() << " QuadIP State ";
    qDebug() << "------------------------------------------";
    qDebug() << "topic name   : " << topic_name;
    qDebug() << "topic time   : " << time;
    qDebug() << "topic clock  : " << clock;
    qDebug() << "------------------------------------------";
    qDebug() << " Header ";
    qDebug() << "------------------------------------------";
    qDebug() << "seq          : " << state.header().seq();
    qDebug() << "timestamp(s) : " << curTime;
    qDebug() << "------------------------------------------";
    qDebug() << "";
#else
    Q_UNUSED(topic_name);
    Q_UNUSED(state);
    Q_UNUSED(time);
    Q_UNUSED(clock);
#endif // PRINT_PUB_SUB_INFO
}

void WolyDataHandler::OnRecvWolyState(const double curTime, const dtproto::woly::WolyState &state)
{
    //OnRecvCpgState(curTime, state.cpgstate());
    OnRecvControlStateActual(curTime, state.controlstateactual());
    OnRecvControlStateDesired(curTime, state.controlstatedesired());
    OnRecvJointState(curTime, state.jointdata());
}

void WolyDataHandler::OnRecvControlStateActual(const double curTime, const dtproto::woly::ControlState &state)
{
    if (_plot_footPos) {
        for (int li = 0; li < legnum; li++) {
            _plot_footPos->AddData(6*li + 3, curTime,
                                state.posbody2footwrtbody(li).x());
            _plot_footPos->AddData(6*li + 4, curTime,
                                state.posbody2footwrtbody(li).y());
            _plot_footPos->AddData(6*li + 5, curTime,
                                state.posbody2footwrtbody(li).z());
        }
        _plot_footPos->DataUpdated(curTime);
    }
}

void WolyDataHandler::OnRecvControlStateDesired(const double curTime, const dtproto::woly::ControlState &state)
{
    if (_plot_footPos) {
        for (int li = 0; li < legnum; li++) {
            _plot_footPos->AddData(6*li + 0, curTime,
                                state.posbody2footwrtbody(li).x());
            _plot_footPos->AddData(6*li + 1, curTime,
                                state.posbody2footwrtbody(li).y());
            _plot_footPos->AddData(6*li + 2, curTime,
                                state.posbody2footwrtbody(li).z());
        }
        _plot_footPos->DataUpdated(curTime);
    }
}

void WolyDataHandler::OnRecvJointState(const double curTime, const dtproto::woly::JointData &state)
{
    if (_plot_jointPos) {
        for (int ji = 0; ji < jdof; ji++) {
            _plot_jointPos->AddData(
                2*ji, curTime, state.joints(ji).despos_rad());
            _plot_jointPos->AddData(
                2*ji+1, curTime, state.joints(ji).actpos_rad());
        }
        _plot_jointPos->DataUpdated(curTime);
    }
    if (_plot_jointVel) {
        for (int ji = 0; ji < jdof; ji++) {
            _plot_jointVel->AddData(
                2*ji, curTime, state.joints(ji).desvel_rps());
            _plot_jointVel->AddData(
                2*ji+1, curTime, state.joints(ji).actvel_rps());
        }
        _plot_jointVel->DataUpdated(curTime);
    }
    if (_plot_jointAcc) {
        for (int ji = 0; ji < jdof; ji++) {
            _plot_jointAcc->AddData(
                2*ji, curTime, state.joints(ji).desacc_rpss());
            _plot_jointAcc->AddData(
                2*ji+1, curTime, state.joints(ji).desacc_rpss());
        }
        _plot_jointAcc->DataUpdated(curTime);
    }
    if (_plot_jointTau) {
        for (int ji = 0; ji < jdof; ji++) {
            _plot_jointTau->AddData(
                2*ji, curTime, state.joints(ji).destorq_nm());
            _plot_jointTau->AddData(
                2*ji+1, curTime, state.joints(ji).acttorq_nm());
        }
        _plot_jointTau->DataUpdated(curTime);
    }
    if (_plot_absEnc) {
        for (int ji = 0; ji < jdof; ji++) {
            _plot_absEnc->AddData(ji, curTime, state.joints(ji).abspos_cnt());
        }
        _plot_absEnc->DataUpdated(curTime);
    }
    if (_plot_incEnc) {
        for (int ji = 0; ji < jdof; ji++) {
            _plot_incEnc->AddData(ji, curTime, state.joints(ji).incpos_cnt());
        }
        _plot_incEnc->DataUpdated(curTime);
    }
}