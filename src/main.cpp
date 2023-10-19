#include "mainwindow.h"
#include "pconstants.h"
#include "plotwindow.h"
#include <QApplication>
#include <QDebug>

//#define USE_TRANSPORT_ECAL
#define USE_TRANSPORT_GRPC
//#define PRINT_PUB_SUB_INFO

#define ENABLE_COM_POS_PLOT
//#define ENABLE_COM_VEL_PLOT
//#define ENABLE_ORIENTATION_PLOT
//#define ENABLE_ANGULAR_VEL_PLOT
//#define ENABLE_FOOT_CONTACT_PLOT
#define ENABLE_CPG_PHI_PLOT
//#define ENABLE_CPG_CPG_PLOT
#define ENABLE_JOINT_POSISION_PLOT
//#define ENABLE_JOINT_VELOCITY_PLOT
//#define ENABLE_JOINT_ACCELERATION_PLOT
//#define ENABLE_JOINT_TORQUE_PLOT
//#define ENABLE_JOINT_ABSOLUTE_ENCODER_PLOT
//#define ENABLE_JOINT_INCREMENTAL_ENCODER_PLOT

#ifdef USE_TRANSPORT_ECAL
#include <ecal/ecal.h>
#include <ecal/msg/protobuf/subscriber.h>
#endif

#ifdef USE_TRANSPORT_GRPC
#include <dtProto/Service.grpc.pb.h>
#include <dtCore/src/dtDAQ/grpc/dtStateSubscriberGrpc.hpp>
#endif

#include "QuadIP.pb.h"


class DataClient {
public:
    DataClient() {}

    void OnRecvQuadIpStateTimeStamped(const char *topic_name,
                                      const dtproto::quadip::QuadIpStateTimeStamped &state,
                                      const long long time, const long long clock)
    {
        double curTime = clock * 1e-3;
        OnRecvQuadIpState(curTime, state.state());

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

    void OnRecvQuadIpState(const double curTime, const dtproto::quadip::QuadIpState &state)
    {
        OnRecvCpgState(curTime, state.cpgstate());
        OnRecvControlStateActual(curTime, state.controlstateactual());
        OnRecvControlStateDesired(curTime, state.controlstatedesired());
        OnRecvJointState(curTime, state.jointstate());
    }

    void OnRecvControlStateActual(const double curTime, const dtproto::quadip::ControlState &state)
    {
        if (_plot_comPos) {
            _plot_comPos->AddData(3, curTime,
                                 state.posworld2comwrtworld().x());
            _plot_comPos->AddData(4, curTime,
                                 state.posworld2comwrtworld().y());
            _plot_comPos->AddData(5, curTime,
                                 state.posworld2comwrtworld().z());
            _plot_comPos->DataUpdated(curTime);
        }
        if (_plot_comVel) {
            _plot_comVel->AddData(3, curTime,
                                 state.velworld2comwrtworld().x());
            _plot_comVel->AddData(4, curTime,
                                 state.velworld2comwrtworld().y());
            _plot_comVel->AddData(5, curTime,
                                 state.velworld2comwrtworld().z());
            _plot_comVel->DataUpdated(curTime);
        }
        if (_plot_orient) {
            _plot_orient->AddData(3, curTime,
                                 state.euleranglebodywrtworld().r());
            _plot_orient->AddData(4, curTime,
                                 state.euleranglebodywrtworld().p());
            _plot_orient->AddData(5, curTime,
                                 state.euleranglebodywrtworld().y());
            _plot_orient->DataUpdated(curTime);
        }
        if (_plot_angVel) {
            _plot_angVel->AddData(3, curTime,
                                 state.angularvelbodywrtworld().r());
            _plot_angVel->AddData(4, curTime,
                                 state.angularvelbodywrtworld().p());
            _plot_angVel->AddData(5, curTime,
                                 state.angularvelbodywrtworld().y());
            _plot_angVel->DataUpdated(curTime);
        }
        if (_plot_contact) {
            _plot_contact->AddData(0, curTime,
                                  (state.contact().a1() ? 1.0 : 0.0));
            _plot_contact->AddData(1, curTime,
                                  (state.contact().a2() ? 1.0 : 0.0));
            _plot_contact->AddData(2, curTime,
                                  (state.contact().a3() ? 1.0 : 0.0));
            _plot_contact->AddData(3, curTime,
                                  (state.contact().a4() ? 1.0 : 0.0));
            _plot_contact->DataUpdated(curTime);
        }
    }

    void OnRecvControlStateDesired(const double curTime, const dtproto::quadip::ControlState &state)
    {
        if (_plot_comPos) {
            _plot_comPos->AddData(0, curTime,
                                 state.posworld2comwrtworld().x());
            _plot_comPos->AddData(1, curTime,
                                 state.posworld2comwrtworld().y());
            _plot_comPos->AddData(2, curTime,
                                 state.posworld2comwrtworld().z());
            _plot_comPos->DataUpdated(curTime);
        }
        if (_plot_comVel) {
            _plot_comVel->AddData(0,curTime,
                                 state.velworld2comwrtworld().x());
            _plot_comVel->AddData(1, curTime,
                                 state.velworld2comwrtworld().y());
            _plot_comVel->AddData(2, curTime,
                                 state.velworld2comwrtworld().z());
            _plot_comVel->DataUpdated(curTime);
        }
        if (_plot_orient) {
            _plot_orient->AddData(0, curTime,
                                 state.euleranglebodywrtworld().r());
            _plot_orient->AddData(1, curTime,
                                 state.euleranglebodywrtworld().p());
            _plot_orient->AddData(2, curTime,
                                 state.euleranglebodywrtworld().y());
            _plot_orient->DataUpdated(curTime);
        }
        if (_plot_angVel) {
            _plot_angVel->AddData(0, curTime,
                                 state.angularvelbodywrtworld().r());
            _plot_angVel->AddData(1, curTime,
                                 state.angularvelbodywrtworld().p());
            _plot_angVel->AddData(2, curTime,
                                 state.angularvelbodywrtworld().y());
            _plot_angVel->DataUpdated(curTime);
        }
    }

    void OnRecvCpgState(const double curTime, const dtproto::quadip::CpgState &state)
    {
        if (_plot_cpgPhi) {
            _plot_cpgPhi->AddData(0, curTime, state.phi().a1());
            _plot_cpgPhi->AddData(1, curTime, state.phi().a2());
            _plot_cpgPhi->AddData(2, curTime, state.phi().a3());
            _plot_cpgPhi->AddData(3, curTime, state.phi().a4());
            _plot_cpgPhi->DataUpdated(curTime);
        }
        if (_plot_cpgCpg) {
            _plot_cpgCpg->AddData(0, curTime, state.cpg().a1());
            _plot_cpgCpg->AddData(1, curTime, state.cpg().a2());
            _plot_cpgCpg->AddData(2, curTime, state.cpg().a3());
            _plot_cpgCpg->AddData(3, curTime, state.cpg().a4());
            _plot_cpgCpg->DataUpdated(curTime);
        }
    }

    void OnRecvJointState(const double curTime, const dtproto::quadip::JointState &state)
    {
        constexpr int jdof = 12;
        if (_plot_jointPos) {
            for (int ji = 0; ji < jdof; ji++) {
                _plot_jointPos->AddData(
                    ji, curTime, state.joint_state_des(ji).position());
                _plot_jointPos->AddData(
                    ji + jdof, curTime, state.joint_state_act(ji).position());
            }
            _plot_jointPos->DataUpdated(curTime);
        }
        if (_plot_jointVel) {
            for (int ji = 0; ji < jdof; ji++) {
                _plot_jointVel->AddData(
                    ji, curTime, state.joint_state_des(ji).velocity());
                _plot_jointVel->AddData(
                    ji + jdof, curTime, state.joint_state_act(ji).velocity());
            }
            _plot_jointVel->DataUpdated(curTime);
        }
        if (_plot_jointAcc) {
            for (int ji = 0; ji < jdof; ji++) {
                _plot_jointAcc->AddData(
                    ji, curTime, state.joint_state_des(ji).acceleration());
                _plot_jointAcc->AddData(
                    ji + jdof, curTime, state.joint_state_act(ji).acceleration());
            }
            _plot_jointAcc->DataUpdated(curTime);
        }
        if (_plot_jointTau) {
            for (int ji = 0; ji < jdof; ji++) {
                _plot_jointTau->AddData(ji, curTime,
                                  state.joint_state_des(ji).torque());
                _plot_jointTau->AddData(
                    ji + jdof, curTime, state.joint_state_act(ji).torque());
            }
            _plot_jointTau->DataUpdated(curTime);
        }
        if (_plot_absEnc) {
            for (int ji = 0; ji < jdof; ji++) {
                _plot_absEnc->AddData(ji, curTime, state.abs_encoder(ji));
            }
            _plot_absEnc->DataUpdated(curTime);
        }
        if (_plot_incEnc) {
            for (int ji = 0; ji < jdof; ji++) {
                _plot_incEnc->AddData(ji, curTime, state.inc_encoder(ji));
            }
            _plot_incEnc->DataUpdated(curTime);
        }
    }

public:
    void SetPlotComPos(PlotWindow* wnd) { _plot_comPos = wnd; }
    void SetPlotComVel(PlotWindow* wnd) { _plot_comVel = wnd; }
    void SetPlotOrient(PlotWindow* wnd) { _plot_orient = wnd; }
    void SetPlotAngVel(PlotWindow* wnd) { _plot_angVel = wnd; }
    void SetPlotContact(PlotWindow* wnd) { _plot_contact = wnd; }
    void SetPlotCpgCpg(PlotWindow* wnd) { _plot_cpgCpg = wnd; }
    void SetPlotCpgPhi(PlotWindow* wnd) { _plot_cpgPhi = wnd; }
    void SetPlotJointPos(PlotWindow* wnd) { _plot_jointPos = wnd; }
    void SetPlotJointVel(PlotWindow* wnd) { _plot_jointVel = wnd; }
    void SetPlotJointAcc(PlotWindow* wnd) { _plot_jointAcc = wnd; }
    void SetPlotJointTau(PlotWindow* wnd) { _plot_jointTau = wnd; }
    void SetPlotAbsEnc(PlotWindow* wnd) { _plot_absEnc = wnd; }
    void SetPlotIncEnc(PlotWindow* wnd) { _plot_incEnc = wnd; }
private:
    PlotWindow *_plot_comPos{nullptr};
    PlotWindow *_plot_comVel{nullptr};
    PlotWindow *_plot_orient{nullptr};
    PlotWindow *_plot_angVel{nullptr};
    PlotWindow *_plot_contact{nullptr};
    PlotWindow *_plot_cpgCpg{nullptr};
    PlotWindow *_plot_cpgPhi{nullptr};
    PlotWindow *_plot_jointPos{nullptr};
    PlotWindow *_plot_jointVel{nullptr};
    PlotWindow *_plot_jointAcc{nullptr};
    PlotWindow *_plot_jointTau{nullptr};
    PlotWindow *_plot_incEnc{nullptr};
    PlotWindow *_plot_absEnc{nullptr};
};




int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow plotToolbox;
    plotToolbox.setWindowTitle("ART Plot (QuadIP)");
    plotToolbox.setWindowFlag(Qt::WindowStaysOnTopHint);
    plotToolbox.show();

#ifdef USE_TRANSPORT_ECAL
    eCAL::Initialize(0, nullptr, "art_plot::QuadIP::ControlStatus");
    eCAL::Process::SetState(proc_sev_healthy, proc_sev_level1, "proc info");
#endif
#ifdef USE_TRANSPORT_GRPC
#endif
    DataClient plotDataHandler;

    /**
     * @brief controlStatePlot
     * ControlStateTimestamp Plot
     */
    std::unique_ptr<PlotWindow> controlStatePlot_comPos;
#ifdef ENABLE_COM_POS_PLOT
    controlStatePlot_comPos = std::unique_ptr<PlotWindow>(new PlotWindow(&plotToolbox));
    // Plot display option 설정
    controlStatePlot_comPos->SetWindowTitle("COM position wrt World");
    controlStatePlot_comPos->AddGraph("Com.x.desired", LineColor<0>());
    controlStatePlot_comPos->AddGraph("Com.y.desired", LineColor<1>());
    controlStatePlot_comPos->AddGraph("Com.z.desired", LineColor<2>());
    controlStatePlot_comPos->AddGraph("Com.x.actual", LineColor<3>());
    controlStatePlot_comPos->AddGraph("Com.y.actual", LineColor<4>());
    controlStatePlot_comPos->AddGraph("Com.z.actual", LineColor<5>());
    controlStatePlot_comPos->show();
    plotToolbox.AddPlot(controlStatePlot_comPos.get());
    plotDataHandler.SetPlotComPos(controlStatePlot_comPos.get());
#endif

    std::unique_ptr<PlotWindow> controlStatePlot_comVel;
#ifdef ENABLE_COM_VEL_PLOT
    controlStatePlot_comVel = std::unique_ptr<PlotWindow>(new PlotWindow(&plotToolbox));
    controlStatePlot_comVel->SetWindowTitle("COM velocity wrt World");
    controlStatePlot_comVel->AddGraph("Com.Vx.desired", LineColor<0>());
    controlStatePlot_comVel->AddGraph("Com.Vy.desired", LineColor<1>());
    controlStatePlot_comVel->AddGraph("Com.Vz.desired", LineColor<2>());
    controlStatePlot_comVel->AddGraph("Com.Vx.actual", LineColor<3>());
    controlStatePlot_comVel->AddGraph("Com.Vy.actual", LineColor<4>());
    controlStatePlot_comVel->AddGraph("Com.Vz.actual", LineColor<5>());
    controlStatePlot_comVel->show();
    plotToolbox.AddPlot(controlStatePlot_comVel.get());
    plotDataHandler.SetPlotComVel(controlStatePlot_comVel.get());
#endif

    std::unique_ptr<PlotWindow> controlStatePlot_orient;
#ifdef ENABLE_ORIENTATION_PLOT
    controlStatePlot_orient = std::unique_ptr<PlotWindow>(new PlotWindow(&plotToolbox));
    controlStatePlot_orient->SetWindowTitle("Body orientation wrt World");
    controlStatePlot_orient->AddGraph("Euler.x.desired", LineColor<6>());
    controlStatePlot_orient->AddGraph("Euler.y.desired", LineColor<7>());
    controlStatePlot_orient->AddGraph("Euler.z.desired", LineColor<8>());
    controlStatePlot_orient->AddGraph("Euler.x.actual", LineColor<9>());
    controlStatePlot_orient->AddGraph("Euler.y.actual", LineColor<10>());
    controlStatePlot_orient->AddGraph("Euler.z.actual", LineColor<11>());
    controlStatePlot_orient->show();
    plotToolbox.AddPlot(controlStatePlot_orient.get());
    plotDataHandler.SetPlotOrient(controlStatePlot_orient.get());
#endif

    std::unique_ptr<PlotWindow> controlStatePlot_angVel;
#ifdef ENABLE_ANGULAR_VEL_PLOT
    controlStatePlot_angVel = std::unique_ptr<PlotWindow>(new PlotWindow(&plotToolbox));
    controlStatePlot_angVel->SetWindowTitle("Body angular velocity wrt World");
    controlStatePlot_angVel->AddGraph("AngularVel.x.desired", LineColor<6>());
    controlStatePlot_angVel->AddGraph("AngularVel.y.desired", LineColor<7>());
    controlStatePlot_angVel->AddGraph("AngularVel.z.desired", LineColor<8>());
    controlStatePlot_angVel->AddGraph("AngularVel.x.actual", LineColor<9>());
    controlStatePlot_angVel->AddGraph("AngularVel.y.actual", LineColor<10>());
    controlStatePlot_angVel->AddGraph("AngularVel.z.actual", LineColor<11>());
    controlStatePlot_angVel->show();
    plotToolbox.AddPlot(controlStatePlot_angVel.get());
    plotDataHandler.SetPlotAngVel(controlStatePlot_angVel.get());
#endif

    std::unique_ptr<PlotWindow> controlStatePlot_contact;
#ifdef ENABLE_FOOT_CONTACT_PLOT
    controlStatePlot_contact = std::unique_ptr<PlotWindow>(new PlotWindow(&plotToolbox));
    controlStatePlot_contact->SetWindowTitle("Foot-floor contact state");
    controlStatePlot_contact->AddGraph("FL", LineColor<12>());
    controlStatePlot_contact->AddGraph("BL", LineColor<13>());
    controlStatePlot_contact->AddGraph("FR", LineColor<14>());
    controlStatePlot_contact->AddGraph("BR", LineColor<15>());
    controlStatePlot_contact->show();
    plotToolbox.AddPlot(controlStatePlot_contact.get());
    plotDataHandler.SetPlotContact(controlStatePlot_contact.get());
#endif


    /**
     * @brief cpgStatePlot
     * CpgStateTimestamp Plot
     */

    std::unique_ptr<PlotWindow> cpgStatePlot_phi;
#ifdef ENABLE_CPG_PHI_PLOT
    cpgStatePlot_phi = std::unique_ptr<PlotWindow>(new PlotWindow(&plotToolbox));
    cpgStatePlot_phi->SetWindowTitle("Cpg.phi");
    cpgStatePlot_phi->AddGraph("Phi.a1", LineColor<10>());
    cpgStatePlot_phi->AddGraph("Phi.a2", LineColor<11>());
    cpgStatePlot_phi->AddGraph("Phi.a3", LineColor<12>());
    cpgStatePlot_phi->AddGraph("Phi.a4", LineColor<13>());
    cpgStatePlot_phi->show();
    plotToolbox.AddPlot(cpgStatePlot_phi.get());
    plotDataHandler.SetPlotCpgPhi(cpgStatePlot_phi.get());
#endif

    std::unique_ptr<PlotWindow> cpgStatePlot_cpg;
#ifdef ENABLE_CPG_CPG_PLOT
    cpgStatePlot_cpg = std::unique_ptr<PlotWindow>(new PlotWindow(&plotToolbox));
    cpgStatePlot_cpg->SetWindowTitle("Cpg.cpg");
    cpgStatePlot_cpg->AddGraph("Cpg.a1", LineColor<10>());
    cpgStatePlot_cpg->AddGraph("Cpg.a2", LineColor<11>());
    cpgStatePlot_cpg->AddGraph("Cpg.a3", LineColor<12>());
    cpgStatePlot_cpg->AddGraph("Cpg.a4", LineColor<13>());
    cpgStatePlot_cpg->show();
    plotToolbox.AddPlot(cpgStatePlot_cpg.get());
    plotDataHandler.SetPlotCpgCpg(cpgStatePlot_cpg.get());
#endif


    /**
     * @brief jointStatePlot
     * JointStateTimestamp Plot
     */
    constexpr int jdof = 12;
    Q_UNUSED(jdof);

    std::unique_ptr<PlotWindow> jointStatePlot_pos;
#ifdef ENABLE_JOINT_POSISION_PLOT
    jointStatePlot_pos = std::unique_ptr<PlotWindow>(new PlotWindow(&plotToolbox));
    jointStatePlot_pos->SetWindowTitle("Joint position");
    for (int ji = 0; ji < jdof; ji++) {
        jointStatePlot_pos->AddGraph(QString("Joint_%02d.pos.desired").arg(ji+1), LineColor(ji));
        jointStatePlot_pos->AddGraph(QString("Joint_%02d.pos.actual").arg(ji+1), LineColor(ji + jdof));
    }
    jointStatePlot_pos->show();
    plotToolbox.AddPlot(jointStatePlot_pos.get());
    plotDataHandler.SetPlotJointPos(jointStatePlot_pos.get());
#endif

    std::unique_ptr<PlotWindow> jointStatePlot_vel;
#ifdef ENABLE_JOINT_VELOCITY_PLOT
    jointStatePlot_vel = std::unique_ptr<PlotWindow>(new PlotWindow(&plotToolbox));
    jointStatePlot_vel->SetWindowTitle("Joint velocity");
    for (int ji = 0; ji < jdof; ji++) {
        jointStatePlot_vel->AddGraph(QString("Joint_%02d.vel.desired").arg(ji+1), LineColor(ji));
        jointStatePlot_vel->AddGraph(QString("Joint_%02d.vel.actual").arg(ji+1), LineColor(ji + jdof));
    }
    jointStatePlot_vel->show();
    plotToolbox.AddPlot(jointStatePlot_vel.get());
    plotDataHandler.SetPlotJointVel(jointStatePlot_vel.get());
#endif

    std::unique_ptr<PlotWindow> jointStatePlot_acc;
#ifdef ENABLE_JOINT_ACCELERATION_PLOT
    jointStatePlot_acc = std::unique_ptr<PlotWindow>(new PlotWindow(&plotToolbox));
    jointStatePlot_acc->SetWindowTitle("Joint acceleration");
    for (int ji = 0; ji < jdof; ji++) {
        jointStatePlot_acc->AddGraph(QString("Joint_%02d.acc.desired").arg(ji+1), LineColor(ji));
        jointStatePlot_acc->AddGraph(QString("Joint_%02d.acc.actual").arg(ji+1),
                                     LineColor(ji + jdof));
    }
    jointStatePlot_acc->show();
    plotToolbox.AddPlot(jointStatePlot_acc.get());
    plotDataHandler.SetPlotJointAcc(jointStatePlot_acc.get());
#endif

    std::unique_ptr<PlotWindow> jointStatePlot_tau;
#ifdef ENABLE_JOINT_TORQUE_PLOT
    jointStatePlot_tau = std::unique_ptr<PlotWindow>(new PlotWindow(&plotToolbox));
    jointStatePlot_tau->SetWindowTitle("Joint torque");
    for (int ji = 0; ji < jdof; ji++) {
        jointStatePlot_tau->AddGraph(QString("Joint_%02d.torque.desired").arg(ji+1), LineColor(ji));
        jointStatePlot_tau->AddGraph(QString("Joint_%02d.torque.actual").arg(ji+1),
                                     LineColor(ji + jdof));
    }
    jointStatePlot_tau->show();
    plotToolbox.AddPlot(jointStatePlot_tau.get());
    plotDataHandler.SetPlotJointTau(jointStatePlot_tau.get());
#endif

    std::unique_ptr<PlotWindow> jointStatePlot_absEnc;
#ifdef ENABLE_JOINT_ABSOLUTE_ENCODER_PLOT
    jointStatePlot_absEnc = std::unique_ptr<PlotWindow>(new PlotWindow(&plotToolbox));
    jointStatePlot_absEnc->SetWindowTitle("Joint absolute encoder");
    for (int ji = 0; ji < jdof; ji++) {
        jointStatePlot_absEnc->AddGraph(QString("Joint_%02d.absolute_encoder").arg(ji+1), LineColor(ji));
    }
    jointStatePlot_absEnc->show();
    plotToolbox.AddPlot(jointStatePlot_absEnc.get());
    plotDataHandler.SetPlotAbsEnc(jointStatePlot_absEnc.get());
#endif

    std::unique_ptr<PlotWindow> jointStatePlot_incEnc;
#ifdef ENABLE_JOINT_INCREMENTAL_ENCODER_PLOT
    jointStatePlot_incEnc = std::unique_ptr<PlotWindow>(new PlotWindow(&plotToolbox));
    jointStatePlot_incEnc->SetWindowTitle("Joint incremental encoder");
    for (int ji = 0; ji < jdof; ji++) {
        jointStatePlot_incEnc->AddGraph(QString("Joint_%02d.incremental_encoder").arg(ji+1), LineColor(ji));
    }
    jointStatePlot_incEnc->show();
    plotToolbox.AddPlot(jointStatePlot_incEnc.get());
    plotDataHandler.SetPlotIncEnc(jointStatePlot_incEnc.get());
#endif


    // 데이터 연결
#ifdef USE_TRANSPORT_ECAL
    eCAL::protobuf::CSubscriber<dtproto::quadip::QuadIpStateTimeStamped>
        sub_quadip_state("RobotState");
    sub_quadip_state.AddReceiveCallback(std::bind(
        &DataClient::OnRecvQuadIpStateTimeStamped, &plotDataHandler,
        std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4));
#endif
#ifdef USE_TRANSPORT_GRPC
    dtCore::dtStateSubscriberGrpc<dtproto::quadip::QuadIpStateTimeStamped> sub_quadip_state("RobotState", "0.0.0.0:50051");

    std::function<void(dtproto::quadip::QuadIpStateTimeStamped&)> handler = [&plotDataHandler](dtproto::quadip::QuadIpStateTimeStamped& msg) {\
        static long long seq = 0;
        plotDataHandler.OnRecvQuadIpStateTimeStamped("", msg, 0, seq++);
    };
    sub_quadip_state.RegMessageHandler(handler);
#endif


    // Start main application(event-loop)
    return app.exec();
}
