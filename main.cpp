#include "mainwindow.h"
#include "pconstants.h"
#include "plotwindow.h"
#include <QApplication>
#include <QDebug>
#include <dtProto/QuadIP.pb.h>
#include <ecal/ecal.h>
#include <ecal/msg/protobuf/subscriber.h>

//#define PRINT_PUB_SUB_INFO

//#define ENABLE_COM_POS_PLOT
//#define ENABLE_COM_VEL_PLOT
//#define ENABLE_ORIENTATION_PLOT
//#define ENABLE_ANGULAR_VEL_PLOT
//#define ENABLE_FOOT_CONTACT_PLOT
#define ENABLE_CPG_PHI_PLOT
#define ENABLE_CPG_CPG_PLOT
//#define ENABLE_JOINT_POSISION_PLOT
//#define ENABLE_JOINT_VELOCITY_PLOT
//#define ENABLE_JOINT_ACCELERATION_PLOT
//#define ENABLE_JOINT_TORQUE_PLOT
//#define ENABLE_JOINT_ABSOLUTE_ENCODER_PLOT
//#define ENABLE_JOINT_INCREMENTAL_ENCODER_PLOT

void OnRecvControlStateActual(
    const char *topic_name,
    const art_protocol::quadip::ControlStateTimeStamped &state,
    const long long time, const long long clock, PlotWindow *plot_comPos,
    PlotWindow *plot_comVel, PlotWindow *plot_orient, PlotWindow *plot_angVel,
    PlotWindow *plot_contact) {
  // double curTime = (double)state.header().time_stamp().seconds() +
  // (double)state.header().time_stamp().nanos() * 1e-9;
  double curTime = clock * 1e-3;
  if (plot_comPos) {
    plot_comPos->graph(3)->addData(curTime,
                                   state.state().posworld2comwrtworld().x());
    plot_comPos->graph(4)->addData(curTime,
                                   state.state().posworld2comwrtworld().y());
    plot_comPos->graph(5)->addData(curTime,
                                   state.state().posworld2comwrtworld().z());
    plot_comPos->DataUpdated(curTime);
  }
  if (plot_comVel) {
    plot_comVel->graph(3)->addData(curTime,
                                   state.state().velworld2comwrtworld().x());
    plot_comVel->graph(4)->addData(curTime,
                                   state.state().velworld2comwrtworld().y());
    plot_comVel->graph(5)->addData(curTime,
                                   state.state().velworld2comwrtworld().z());
    plot_comVel->DataUpdated(curTime);
  }
  if (plot_orient) {
    plot_orient->graph(3)->addData(curTime,
                                   state.state().euleranglebodywrtworld().r());
    plot_orient->graph(4)->addData(curTime,
                                   state.state().euleranglebodywrtworld().p());
    plot_orient->graph(5)->addData(curTime,
                                   state.state().euleranglebodywrtworld().y());
    plot_orient->DataUpdated(curTime);
  }
  if (plot_angVel) {
    plot_angVel->graph(3)->addData(curTime,
                                   state.state().angularvelbodywrtworld().r());
    plot_angVel->graph(4)->addData(curTime,
                                   state.state().angularvelbodywrtworld().p());
    plot_angVel->graph(5)->addData(curTime,
                                   state.state().angularvelbodywrtworld().y());
    plot_angVel->DataUpdated(curTime);
  }
  if (plot_contact) {
    plot_contact->graph(0)->addData(curTime,
                                    (state.state().contact().a1() ? 1.0 : 0.0));
    plot_contact->graph(1)->addData(curTime,
                                    (state.state().contact().a2() ? 1.0 : 0.0));
    plot_contact->graph(2)->addData(curTime,
                                    (state.state().contact().a3() ? 1.0 : 0.0));
    plot_contact->graph(3)->addData(curTime,
                                    (state.state().contact().a4() ? 1.0 : 0.0));
    plot_contact->DataUpdated(curTime);
  }

#ifdef PRINT_PUB_SUB_INFO
  qDebug() << "------------------------------------------";
  qDebug() << " QuadIP Control State ";
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

void OnRecvControlStateDesired(
    const char *topic_name,
    const art_protocol::quadip::ControlStateTimeStamped &state,
    const long long time, const long long clock, PlotWindow *plot_comPos,
    PlotWindow *plot_comVel, PlotWindow *plot_orient, PlotWindow *plot_angVel) {
  // double curTime = (double)state.header().time_stamp().seconds() +
  // (double)state.header().time_stamp().nanos() * 1e-9;
  double curTime = clock * 1e-3;
  if (plot_comPos) {
    plot_comPos->graph(0)->addData(curTime,
                                   state.state().posworld2comwrtworld().x());
    plot_comPos->graph(1)->addData(curTime,
                                   state.state().posworld2comwrtworld().y());
    plot_comPos->graph(2)->addData(curTime,
                                   state.state().posworld2comwrtworld().z());
    plot_comPos->DataUpdated(curTime);
  }
  if (plot_comVel) {
    plot_comVel->graph(0)->addData(curTime,
                                   state.state().velworld2comwrtworld().x());
    plot_comVel->graph(1)->addData(curTime,
                                   state.state().velworld2comwrtworld().y());
    plot_comVel->graph(2)->addData(curTime,
                                   state.state().velworld2comwrtworld().z());
    plot_comVel->DataUpdated(curTime);
  }
  if (plot_orient) {
    plot_orient->graph(0)->addData(curTime,
                                   state.state().euleranglebodywrtworld().r());
    plot_orient->graph(1)->addData(curTime,
                                   state.state().euleranglebodywrtworld().p());
    plot_orient->graph(2)->addData(curTime,
                                   state.state().euleranglebodywrtworld().y());
    plot_orient->DataUpdated(curTime);
  }
  if (plot_angVel) {
    plot_angVel->graph(0)->addData(curTime,
                                   state.state().angularvelbodywrtworld().r());
    plot_angVel->graph(1)->addData(curTime,
                                   state.state().angularvelbodywrtworld().p());
    plot_angVel->graph(2)->addData(curTime,
                                   state.state().angularvelbodywrtworld().y());
    plot_angVel->DataUpdated(curTime);
  }

#ifdef PRINT_PUB_SUB_INFO
  qDebug() << "------------------------------------------";
  qDebug() << " QuadIP Control State ";
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

void OnRecvCpgState(const char *topic_name,
                    const art_protocol::quadip::CpgStateTimeStamped &state,
                    const long long time, const long long clock,
                    PlotWindow *plot_phi, PlotWindow *plot_cpg) {
  // double curTime = (double)state.header().time_stamp().seconds() +
  // (double)state.header().time_stamp().nanos() * 1e-9;
  double curTime = clock * 1e-3;
  if (plot_phi) {
    plot_phi->graph(0)->addData(curTime, state.state().phi().a1());
    plot_phi->graph(1)->addData(curTime, state.state().phi().a2());
    plot_phi->graph(2)->addData(curTime, state.state().phi().a3());
    plot_phi->graph(3)->addData(curTime, state.state().phi().a4());
    plot_phi->DataUpdated(curTime);
  }
  if (plot_cpg) {
    plot_cpg->graph(0)->addData(curTime, state.state().cpg().a1());
    plot_cpg->graph(1)->addData(curTime, state.state().cpg().a2());
    plot_cpg->graph(2)->addData(curTime, state.state().cpg().a3());
    plot_cpg->graph(3)->addData(curTime, state.state().cpg().a4());
    plot_cpg->DataUpdated(curTime);
  }

#ifdef PRINT_PUB_SUB_INFO
  qDebug() << "------------------------------------------";
  qDebug() << " QuadIP Cpg State ";
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

void OnRecvJointState(const char *topic_name,
                      const art_protocol::quadip::JointStateTimeStamped &state,
                      const long long time, const long long clock,
                      PlotWindow *plot_pos, PlotWindow *plot_vel,
                      PlotWindow *plot_acc, PlotWindow *plot_tau,
                      PlotWindow *plot_absEnc, PlotWindow *plot_incEnc) {
  constexpr int jdof = 12;
  // double curTime = (double)state.header().time_stamp().seconds() +
  // (double)state.header().time_stamp().nanos() * 1e-9;
  double curTime = clock * 1e-3;

  if (plot_pos) {
    for (int ji = 0; ji < jdof; ji++) {
      plot_pos->graph(ji)->addData(
          curTime, state.state().joint_state_des(ji).position());
      plot_pos->graph(ji + jdof)->addData(
          curTime, state.state().joint_state_act(ji).position());
    }
    plot_pos->DataUpdated(curTime);
  }
  if (plot_vel) {
    for (int ji = 0; ji < jdof; ji++) {
      plot_vel->graph(ji)->addData(
          curTime, state.state().joint_state_des(ji).velocity());
      plot_vel->graph(ji + jdof)->addData(
          curTime, state.state().joint_state_act(ji).velocity());
    }
    plot_vel->DataUpdated(curTime);
  }
  if (plot_acc) {
    for (int ji = 0; ji < jdof; ji++) {
      plot_acc->graph(ji)->addData(
          curTime, state.state().joint_state_des(ji).acceleration());
      plot_acc->graph(ji + jdof)->addData(
          curTime, state.state().joint_state_act(ji).acceleration());
    }
    plot_acc->DataUpdated(curTime);
  }
  if (plot_tau) {
    for (int ji = 0; ji < jdof; ji++) {
      plot_tau->graph(ji)->addData(curTime,
                                   state.state().joint_state_des(ji).torque());
      plot_tau->graph(ji + jdof)->addData(
          curTime, state.state().joint_state_act(ji).torque());
    }
    plot_tau->DataUpdated(curTime);
  }
  if (plot_absEnc) {
    for (int ji = 0; ji < jdof; ji++) {
      plot_absEnc->graph(ji)->addData(curTime, state.state().abs_encoder(ji));
    }
    plot_absEnc->DataUpdated(curTime);
  }
  if (plot_incEnc) {
    for (int ji = 0; ji < jdof; ji++) {
      plot_incEnc->graph(ji)->addData(curTime, state.state().inc_encoder(ji));
    }
    plot_incEnc->DataUpdated(curTime);
  }

#ifdef PRINT_PUB_SUB_INFO
  qDebug() << "------------------------------------------";
  qDebug() << " QuadIP Joint State ";
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

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //    MainWindow w;
    //    w.show();

    eCAL::Initialize(0, nullptr, "art_plot::QuadIP::ControlStatus");
    eCAL::Process::SetState(proc_sev_healthy, proc_sev_level1, "proc info");

    /**
     * @brief controlStatePlot
     * ControlStateTimestamp Plot
     */
    std::unique_ptr<PlotWindow> controlStatePlot_comPos;
#ifdef ENABLE_COM_POS_PLOT
    controlStatePlot_comPos = std::unique_ptr<PlotWindow>(new PlotWindow());
    // Plot display option 설정
    controlStatePlot_comPos->SetWindowTitle("COM position wrt World");
    controlStatePlot_comPos->AutoScale(true);
    controlStatePlot_comPos->AutoScroll(true);
    controlStatePlot_comPos->AutoScrollWindow(5.0);
    controlStatePlot_comPos->AddGraph("Com.x.desired", LineColor<0>());
    controlStatePlot_comPos->AddGraph("Com.y.desired", LineColor<1>());
    controlStatePlot_comPos->AddGraph("Com.z.desired", LineColor<2>());
    controlStatePlot_comPos->AddGraph("Com.x.actual", LineColor<3>());
    controlStatePlot_comPos->AddGraph("Com.y.actual", LineColor<4>());
    controlStatePlot_comPos->AddGraph("Com.z.actual", LineColor<5>());
    controlStatePlot_comPos->show();
#endif

    std::unique_ptr<PlotWindow> controlStatePlot_comVel;
#ifdef ENABLE_COM_VEL_PLOT
    controlStatePlot_comVel = std::unique_ptr<PlotWindow>(new PlotWindow());
    controlStatePlot_comVel->SetWindowTitle("COM velocity wrt World");
    controlStatePlot_comVel->AutoScale(true);
    controlStatePlot_comVel->AutoScroll(true);
    controlStatePlot_comVel->AutoScrollWindow(5.0);
    controlStatePlot_comVel->AddGraph("Com.Vx.desired", LineColor<0>());
    controlStatePlot_comVel->AddGraph("Com.Vy.desired", LineColor<1>());
    controlStatePlot_comVel->AddGraph("Com.Vz.desired", LineColor<2>());
    controlStatePlot_comVel->AddGraph("Com.Vx.actual", LineColor<3>());
    controlStatePlot_comVel->AddGraph("Com.Vy.actual", LineColor<4>());
    controlStatePlot_comVel->AddGraph("Com.Vz.actual", LineColor<5>());
    controlStatePlot_comVel->show();
#endif

    std::unique_ptr<PlotWindow> controlStatePlot_orient;
#ifdef ENABLE_ORIENTATION_PLOT
    controlStatePlot_orient = std::unique_ptr<PlotWindow>(new PlotWindow());
    controlStatePlot_orient->SetWindowTitle("Body orientation wrt World");
    controlStatePlot_orient->AutoScale(true);
    controlStatePlot_orient->AutoScroll(true);
    controlStatePlot_orient->AutoScrollWindow(5.0);
    controlStatePlot_orient->AddGraph("Euler.x.desired", LineColor<6>());
    controlStatePlot_orient->AddGraph("Euler.y.desired", LineColor<7>());
    controlStatePlot_orient->AddGraph("Euler.z.desired", LineColor<8>());
    controlStatePlot_orient->AddGraph("Euler.x.actual", LineColor<9>());
    controlStatePlot_orient->AddGraph("Euler.y.actual", LineColor<10>());
    controlStatePlot_orient->AddGraph("Euler.z.actual", LineColor<11>());
    controlStatePlot_orient->show();
#endif

    std::unique_ptr<PlotWindow> controlStatePlot_angVel;
#ifdef ENABLE_ANGULAR_VEL_PLOT
    controlStatePlot_angVel = std::unique_ptr<PlotWindow>(new PlotWindow());
    controlStatePlot_angVel->SetWindowTitle("Body angular velocity wrt World");
    controlStatePlot_angVel->AutoScale(true);
    controlStatePlot_angVel->AutoScroll(true);
    controlStatePlot_angVel->AutoScrollWindow(5.0);
    controlStatePlot_angVel->AddGraph("AngularVel.x.desired", LineColor<6>());
    controlStatePlot_angVel->AddGraph("AngularVel.y.desired", LineColor<7>());
    controlStatePlot_angVel->AddGraph("AngularVel.z.desired", LineColor<8>());
    controlStatePlot_angVel->AddGraph("AngularVel.x.actual", LineColor<9>());
    controlStatePlot_angVel->AddGraph("AngularVel.y.actual", LineColor<10>());
    controlStatePlot_angVel->AddGraph("AngularVel.z.actual", LineColor<11>());
    controlStatePlot_angVel->show();
#endif

    std::unique_ptr<PlotWindow> controlStatePlot_contact;
#ifdef ENABLE_FOOT_CONTACT_PLOT
    controlStatePlot_contact = std::unique_ptr<PlotWindow>(new PlotWindow());
    controlStatePlot_contact->SetWindowTitle("Foot-floor contact state");
    controlStatePlot_contact->AutoScale(true);
    controlStatePlot_contact->AutoScroll(true);
    controlStatePlot_contact->AutoScrollWindow(5.0);
    controlStatePlot_contact->AddGraph("FL", LineColor<12>());
    controlStatePlot_contact->AddGraph("BL", LineColor<13>());
    controlStatePlot_contact->AddGraph("FR", LineColor<14>());
    controlStatePlot_contact->AddGraph("BR", LineColor<15>());
    controlStatePlot_contact->show();
#endif

    // 데이터 연결
    eCAL::protobuf::CSubscriber<art_protocol::quadip::ControlStateTimeStamped>
        sub_control_state_act("ControlStateActual");
    eCAL::protobuf::CSubscriber<art_protocol::quadip::ControlStateTimeStamped>
        sub_control_state_des("ControlStateDesired");
    sub_control_state_act.AddReceiveCallback(std::bind(
        OnRecvControlStateActual, std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4,
        controlStatePlot_comPos.get(), controlStatePlot_comVel.get(),
        controlStatePlot_orient.get(), controlStatePlot_angVel.get(),
        controlStatePlot_contact.get()));
    sub_control_state_des.AddReceiveCallback(std::bind(
        OnRecvControlStateDesired, std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4,
        controlStatePlot_comPos.get(), controlStatePlot_comVel.get(),
        controlStatePlot_orient.get(), controlStatePlot_angVel.get()));

    /**
     * @brief cpgStatePlot
     * CpgStateTimestamp Plot
     */

    std::unique_ptr<PlotWindow> cpgStatePlot_phi;
#ifdef ENABLE_CPG_PHI_PLOT
    cpgStatePlot_phi = std::unique_ptr<PlotWindow>(new PlotWindow());
    cpgStatePlot_phi->SetWindowTitle("Cpg.phi");
    cpgStatePlot_phi->ShowLegend(false);
    cpgStatePlot_phi->AutoScroll(true);
    cpgStatePlot_phi->AutoScrollWindow(3.0);
    cpgStatePlot_phi->SetXRange(0.0, 10.0);
    cpgStatePlot_phi->AutoScale(true);
    cpgStatePlot_phi->SetYRange(-2.5, 2.5);
    cpgStatePlot_phi->AddGraph("Phi.a1", LineColor<10>());
    cpgStatePlot_phi->AddGraph("Phi.a2", LineColor<11>());
    cpgStatePlot_phi->AddGraph("Phi.a3", LineColor<12>());
    cpgStatePlot_phi->AddGraph("Phi.a4", LineColor<13>());
    cpgStatePlot_phi->show();
#endif

    std::unique_ptr<PlotWindow> cpgStatePlot_cpg;
#ifdef ENABLE_CPG_CPG_PLOT
    cpgStatePlot_cpg = std::unique_ptr<PlotWindow>(new PlotWindow());
    cpgStatePlot_cpg->SetWindowTitle("Cpg.cpg");
    cpgStatePlot_cpg->AutoScale(true);
    cpgStatePlot_cpg->AutoScroll(true);
    cpgStatePlot_cpg->AutoScrollWindow(5.0);
    cpgStatePlot_cpg->AddGraph("Cpg.a1", LineColor<10>());
    cpgStatePlot_cpg->AddGraph("Cpg.a2", LineColor<11>());
    cpgStatePlot_cpg->AddGraph("Cpg.a3", LineColor<12>());
    cpgStatePlot_cpg->AddGraph("Cpg.a4", LineColor<13>());
    cpgStatePlot_cpg->show();
#endif

    eCAL::protobuf::CSubscriber<art_protocol::quadip::CpgStateTimeStamped>
        sub_cpg_state("CpgState");
    sub_cpg_state.AddReceiveCallback(
        std::bind(OnRecvCpgState, std::placeholders::_1, std::placeholders::_2,
                  std::placeholders::_3, std::placeholders::_4,
                  cpgStatePlot_phi.get(), cpgStatePlot_cpg.get()));

    /**
     * @brief jointStatePlot
     * JointStateTimestamp Plot
     */
    constexpr int jdof = 12;
    Q_UNUSED(jdof);

    std::unique_ptr<PlotWindow> jointStatePlot_pos;
#ifdef ENABLE_JOINT_POSISION_PLOT
    jointStatePlot_pos = std::unique_ptr<PlotWindow>(new PlotWindow());
    jointStatePlot_pos->SetWindowTitle("Joint position");
    jointStatePlot_pos->AutoScale(true);
    jointStatePlot_pos->AutoScroll(true);
    jointStatePlot_pos->AutoScrollWindow(5.0);
    for (int ji = 0; ji < jdof; ji++) {
        jointStatePlot_pos->AddGraph(QString("Joint_%02d.pos.desired").arg(ji+1), LineColor(ji));
        jointStatePlot_pos->AddGraph(QString("Joint_%02d.pos.actual").arg(ji+1), LineColor(ji + jdof));
    }
    jointStatePlot_pos->show();
#endif

    std::unique_ptr<PlotWindow> jointStatePlot_vel;
#ifdef ENABLE_JOINT_VELOCITY_PLOT
    jointStatePlot_vel = std::unique_ptr<PlotWindow>(new PlotWindow());
    jointStatePlot_vel->SetWindowTitle("Joint velocity");
    jointStatePlot_vel->AutoScale(true);
    jointStatePlot_vel->AutoScroll(true);
    jointStatePlot_vel->AutoScrollWindow(5.0);
    for (int ji = 0; ji < jdof; ji++) {
        jointStatePlot_vel->AddGraph(QString("Joint_%02d.vel.desired").arg(ji+1), LineColor(ji));
        jointStatePlot_vel->AddGraph(QString("Joint_%02d.vel.actual").arg(ji+1), LineColor(ji + jdof));
    }
    jointStatePlot_vel->show();
#endif

    std::unique_ptr<PlotWindow> jointStatePlot_acc;
#ifdef ENABLE_JOINT_ACCELERATION_PLOT
    jointStatePlot_acc = std::unique_ptr<PlotWindow>(new PlotWindow());
    jointStatePlot_acc->SetWindowTitle("Joint acceleration");
    jointStatePlot_acc->AutoScale(true);
    jointStatePlot_acc->AutoScroll(true);
    jointStatePlot_acc->AutoScrollWindow(5.0);
    for (int ji = 0; ji < jdof; ji++) {
        jointStatePlot_acc->AddGraph(QString("Joint_%02d.acc.desired").arg(ji+1), LineColor(ji));
        jointStatePlot_acc->AddGraph(QString("Joint_%02d.acc.actual").arg(ji+1),
                                     LineColor(ji + jdof));
    }
    jointStatePlot_acc->show();
#endif

    std::unique_ptr<PlotWindow> jointStatePlot_tau;
#ifdef ENABLE_JOINT_TORQUE_PLOT
    jointStatePlot_tau = std::unique_ptr<PlotWindow>(new PlotWindow());
    jointStatePlot_tau->SetWindowTitle("Joint torque");
    jointStatePlot_tau->AutoScale(true);
    jointStatePlot_tau->AutoScroll(true);
    jointStatePlot_tau->AutoScrollWindow(5.0);
    for (int ji = 0; ji < jdof; ji++) {
        jointStatePlot_tau->AddGraph(QString("Joint_%02d.torque.desired").arg(ji+1), LineColor(ji));
        jointStatePlot_tau->AddGraph(QString("Joint_%02d.torque.actual").arg(ji+1),
                                     LineColor(ji + jdof));
    }
    jointStatePlot_tau->show();
#endif

    std::unique_ptr<PlotWindow> jointStatePlot_absEnc;
#ifdef ENABLE_JOINT_ABSOLUTE_ENCODER_PLOT
    jointStatePlot_absEnc = std::unique_ptr<PlotWindow>(new PlotWindow());
    jointStatePlot_absEnc->SetWindowTitle("Joint absolute encoder");
    jointStatePlot_absEnc->AutoScale(true);
    jointStatePlot_absEnc->AutoScroll(true);
    jointStatePlot_absEnc->AutoScrollWindow(5.0);
    for (int ji = 0; ji < jdof; ji++) {
        jointStatePlot_absEnc->AddGraph(QString("Joint_%02d.absolute_encoder").arg(ji+1), LineColor(ji));
    }
    jointStatePlot_absEnc->show();
#endif

    std::unique_ptr<PlotWindow> jointStatePlot_incEnc;
#ifdef ENABLE_JOINT_INCREMENTAL_ENCODER_PLOT
    jointStatePlot_incEnc = std::unique_ptr<PlotWindow>(new PlotWindow());
    jointStatePlot_incEnc->SetWindowTitle("Joint incremental encoder");
    jointStatePlot_incEnc->AutoScale(true);
    jointStatePlot_incEnc->AutoScroll(true);
    jointStatePlot_incEnc->AutoScrollWindow(5.0);
    for (int ji = 0; ji < jdof; ji++) {
        jointStatePlot_incEnc->AddGraph(QString("Joint_%02d.incremental_encoder").arg(ji+1), LineColor(ji));
    }
    jointStatePlot_incEnc->show();
#endif

    eCAL::protobuf::CSubscriber<art_protocol::quadip::JointStateTimeStamped>
        sub_joint_state("JointState");
    sub_joint_state.AddReceiveCallback(std::bind(
        OnRecvJointState, std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4, jointStatePlot_pos.get(),
        jointStatePlot_vel.get(), jointStatePlot_acc.get(),
        jointStatePlot_tau.get(), jointStatePlot_absEnc.get(),
        jointStatePlot_incEnc.get()));

    // Start main application(event-loop)
    return app.exec();
}
