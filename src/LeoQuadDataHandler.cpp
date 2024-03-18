#include "LeoQuadDataHandler.h"
#include "cmdopts.hpp"
#include "mainwindow.h"
#include "pconstants.h"
#include "plotwindow.h"
#include "stringutil.hpp"
#include <QApplication>
#include <QDebug>
#include <dtCore/src/dtLog/dtLog.h>

// #define PRINT_PUB_SUB_INFO

#define ENABLE_COM_POS_PLOT
#define ENABLE_COM_VEL_PLOT
#define ENABLE_ORIENTATION_PLOT
#define ENABLE_ANGULAR_VEL_PLOT
#define ENABLE_FOOT_POS_PLOT
#define ENABLE_FOOT_VEL_PLOT
// #define ENABLE_FOOT_CONTACT_PLOT
#define ENABLE_FOOT_FORCE_PLOT
// #define ENABLE_CPG_PHI_PLOT
#define ENABLE_CPG_CPG_PLOT
#define ENABLE_JOINT_POSISION_PLOT
#define ENABLE_JOINT_VELOCITY_PLOT
// #define ENABLE_JOINT_ACCELERATION_PLOT
#define ENABLE_JOINT_TORQUE_PLOT
// #define ENABLE_JOINT_ABSOLUTE_ENCODER_PLOT
// #define ENABLE_JOINT_INCREMENTAL_ENCODER_PLOT
// #define ENABLE_CTRL2COM_POS_PLOT
#define ENABLE_EEPOSG_G_PLOT
// #define ENABLE_CTRL2COM_VEL_PLOT
#define ENABLE_EEVELE_G_PLOT

#define ENABLE_THREAD_STATE_PLOT
#define ENABLE_DEBUG_DATA_PLOT

constexpr static int jdof = 12;
constexpr static int legnum = 4;

static const double RAD2DEG = 57.295779513; //! 180.0f/M_PI
static const double DEG2RAD = 0.0174532925; //! M_PI/180.0f

LeoQuadDataHandler::LeoQuadDataHandler(MainWindow *plotToolbox)
    : DataHandler(plotToolbox)
#ifdef ENABLE_COM_POS_PLOT
      ,
      _plot_comPos(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_COM_VEL_PLOT
      ,
      _plot_comVel(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_ORIENTATION_PLOT
      ,
      _plot_orient(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_ANGULAR_VEL_PLOT
      ,
      _plot_angVel(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_FOOT_POS_PLOT
      ,
      _plot_footPos(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_FOOT_VEL_PLOT
      ,
      _plot_footVel(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_FOOT_CONTACT_PLOT
      ,
      _plot_contact(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_FOOT_FORCE_PLOT
      ,
      _plot_footForce(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_CPG_CPG_PLOT
      ,
      _plot_cpgCpg(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_CPG_PHI_PLOT
      ,
      _plot_cpgPhi(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_JOINT_POSISION_PLOT
      ,
      _plot_jointPos(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_JOINT_VELOCITY_PLOT
      ,
      _plot_jointVel(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_JOINT_ACCELERATION_PLOT
      ,
      _plot_jointAcc(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_JOINT_TORQUE_PLOT
      ,
      _plot_jointTau(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_JOINT_INCREMENTAL_ENCODER_PLOT
      ,
      _plot_incEnc(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_JOINT_ABSOLUTE_ENCODER_PLOT
      ,
      _plot_absEnc(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_EEPOSG_G_PLOT
      ,
      _plot_eePosG_G(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_EEVELE_G_PLOT
      ,
      _plot_eeVelE_G(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_THREAD_STATE_PLOT
      ,
      _plot_threadState(std::make_unique<PlotWindow>(plotToolbox))
#endif
#ifdef ENABLE_DEBUG_DATA_PLOT
      ,
      _plot_debugData(std::make_unique<PlotWindow>(plotToolbox))
#endif
{
    LOG(info) << "LeoQuadDataHandler created.";

    BuildPlots();
}

LeoQuadDataHandler::~LeoQuadDataHandler()
{
#ifdef USE_TRANSPORT_GRPC
    _sub_reconnector_running = false;
    if (_sub_reconnector.joinable())
        _sub_reconnector.join();
#endif
}

void LeoQuadDataHandler::BuildPlots()
{
#ifdef ENABLE_COM_POS_PLOT
    //_plot_comPos = std::make_unique<PlotWindow>(_plotToolbox);
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
    // plotDataHandler.SetPlotOrient(_plot_orient.get());
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

#ifdef ENABLE_FOOT_POS_PLOT
    // _plot_footPos = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_footPos->SetWindowTitle("posBody2Foot wrt Body");
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

#ifdef ENABLE_FOOT_VEL_PLOT
    // _plot_footVel = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_footVel->SetWindowTitle("velBody2Foot wrt Body");
    _plot_footVel->AddGraph("FL.x.desired", LineColor<0>());
    _plot_footVel->AddGraph("FL.y.desired", LineColor<1>());
    _plot_footVel->AddGraph("FL.z.desired", LineColor<2>());
    _plot_footVel->AddGraph("FL.x.actual", LineColor<3>());
    _plot_footVel->AddGraph("FL.y.actual", LineColor<4>());
    _plot_footVel->AddGraph("FL.z.actual", LineColor<5>());
    _plot_footVel->AddGraph("BL.x.desired", LineColor<6>());
    _plot_footVel->AddGraph("BL.y.desired", LineColor<7>());
    _plot_footVel->AddGraph("BL.z.desired", LineColor<8>());
    _plot_footVel->AddGraph("BL.x.actual", LineColor<9>());
    _plot_footVel->AddGraph("BL.y.actual", LineColor<10>());
    _plot_footVel->AddGraph("BL.z.actual", LineColor<11>());
    _plot_footVel->AddGraph("FR.x.desired", LineColor<12>());
    _plot_footVel->AddGraph("FR.y.desired", LineColor<13>());
    _plot_footVel->AddGraph("FR.z.desired", LineColor<14>());
    _plot_footVel->AddGraph("FR.x.actual", LineColor<15>());
    _plot_footVel->AddGraph("FR.y.actual", LineColor<16>());
    _plot_footVel->AddGraph("FR.z.actual", LineColor<17>());
    _plot_footVel->AddGraph("BR.x.desired", LineColor<18>());
    _plot_footVel->AddGraph("BR.y.desired", LineColor<19>());
    _plot_footVel->AddGraph("BR.z.desired", LineColor<20>());
    _plot_footVel->AddGraph("BR.x.actual", LineColor<21>());
    _plot_footVel->AddGraph("BR.y.actual", LineColor<22>());
    _plot_footVel->AddGraph("BR.z.actual", LineColor<23>());
    _plot_footVel->show();
    RegisterPlot(_plot_footVel.get());
#endif

#ifdef ENABLE_FOOT_FORCE_PLOT
    // _plot_footForce = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_footForce->SetWindowTitle("Foot force wrt World");
    _plot_footForce->AddGraph("FL.x.desired", LineColor<0>());
    _plot_footForce->AddGraph("FL.y.desired", LineColor<1>());
    _plot_footForce->AddGraph("FL.z.desired", LineColor<2>());
    _plot_footForce->AddGraph("BL.x.desired", LineColor<3>());
    _plot_footForce->AddGraph("BL.y.desired", LineColor<4>());
    _plot_footForce->AddGraph("BL.z.desired", LineColor<5>());
    _plot_footForce->AddGraph("FR.x.desired", LineColor<6>());
    _plot_footForce->AddGraph("FR.y.desired", LineColor<7>());
    _plot_footForce->AddGraph("FR.z.desired", LineColor<8>());
    _plot_footForce->AddGraph("BR.x.desired", LineColor<9>());
    _plot_footForce->AddGraph("BR.y.desired", LineColor<10>());
    _plot_footForce->AddGraph("BR.z.desired", LineColor<11>());
    _plot_footForce->show();
    RegisterPlot(_plot_footForce.get());
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
    // _plot_cpgPhi = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_cpgPhi->SetWindowTitle("Cpg.phi");
    _plot_cpgPhi->AddGraph("Phi.a1", LineColor<10>());
    _plot_cpgPhi->AddGraph("Phi.a2", LineColor<11>());
    _plot_cpgPhi->AddGraph("Phi.a3", LineColor<12>());
    _plot_cpgPhi->AddGraph("Phi.a4", LineColor<13>());
    _plot_cpgPhi->show();
    RegisterPlot(_plot_phi.get());
#endif

#ifdef ENABLE_CPG_CPG_PLOT
    // _plot_cpgCpg = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_cpgCpg->SetWindowTitle("Cpg.cpg");
    _plot_cpgCpg->AddGraph("Cpg.a1", LineColor<10>());
    _plot_cpgCpg->AddGraph("Cpg.a2", LineColor<11>());
    _plot_cpgCpg->AddGraph("Cpg.a3", LineColor<12>());
    _plot_cpgCpg->AddGraph("Cpg.a4", LineColor<13>());
    _plot_cpgCpg->show();
    RegisterPlot(_plot_cpgCpg.get());
#endif

#ifdef ENABLE_JOINT_POSISION_PLOT
    //_plot_jointPos = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_jointPos->SetWindowTitle("Joint position");
    for (int ji = 0; ji < jdof; ji++)
    {
        _plot_jointPos->AddGraph(QString("Joint_%1.pos.desired").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji));
        _plot_jointPos->AddGraph(QString("Joint_%1.pos.actual").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji + jdof));
    }
    _plot_jointPos->show();
    RegisterPlot(_plot_jointPos.get());
#endif

#ifdef ENABLE_JOINT_VELOCITY_PLOT
    // _plot_jointVel = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_jointVel->SetWindowTitle("Joint velocity");
    for (int ji = 0; ji < jdof; ji++)
    {
        _plot_jointVel->AddGraph(QString("Joint_%1.vel.desired").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji));
        _plot_jointVel->AddGraph(QString("Joint_%1.vel.actual").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji + jdof));
    }
    _plot_jointVel->show();
    RegisterPlot(_plot_jointVel.get());
#endif

#ifdef ENABLE_JOINT_ACCELERATION_PLOT
    // _plot_jointAcc = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_jointAcc->SetWindowTitle("Joint acceleration");
    for (int ji = 0; ji < jdof; ji++)
    {
        _plot_jointAcc->AddGraph(QString("Joint_%1.acc.desired").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji));
        _plot_jointAcc->AddGraph(QString("Joint_%1.acc.actual").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji + jdof));
    }
    _plot_jointAcc->show();
    RegisterPlot(_plot_jointAcc.get());
    plotDataHandler.SetPlotJointAcc(_plot_jointAcc.get());
#endif

#ifdef ENABLE_JOINT_TORQUE_PLOT
    // _plot_jointTau = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_jointTau->SetWindowTitle("Joint torque");
    for (int ji = 0; ji < jdof; ji++)
    {
        _plot_jointTau->AddGraph(QString("Joint_%1.tau.desired").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji));
        _plot_jointTau->AddGraph(QString("Joint_%1.tau.actual").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji + jdof));
    }
    _plot_jointTau->show();
    RegisterPlot(_plot_jointTau.get());
#endif

#ifdef ENABLE_JOINT_ABSOLUTE_ENCODER_PLOT
    // _plot_absEnc = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_absEnc->SetWindowTitle("Joint absolute encoder");
    for (int ji = 0; ji < jdof; ji++)
    {
        _plot_absEnc->AddGraph(QString("Joint_%1.absolute_encoder").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji));
    }
    _plot_absEnc->show();
    RegisterPlot(_plot_absEnc.get());
#endif

#ifdef ENABLE_JOINT_INCREMENTAL_ENCODER_PLOT
    // _plot_incEnc = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_incEnc->SetWindowTitle("Joint incremental encoder");
    for (int ji = 0; ji < jdof; ji++)
    {
        _plot_incEnc->AddGraph(QString("Joint_%1.incremental_encoder").arg(ji + 1, 2, 10, QLatin1Char('0')), LineColor(ji));
    }
    _plot_incEnc->show();
    RegisterPlot(_plot_incEnc.get());
#endif

#ifdef ENABLE_EEPOSG_G_PLOT
    // _plot_posctrl2com = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_eePosG_G->SetWindowTitle("eePosG_G");
    _plot_eePosG_G->AddGraph("FL.x.desired", LineColor<0>());
    _plot_eePosG_G->AddGraph("FL.y.desired", LineColor<1>());
    _plot_eePosG_G->AddGraph("FL.z.desired", LineColor<2>());
    _plot_eePosG_G->AddGraph("FL.x.actual", LineColor<3>());
    _plot_eePosG_G->AddGraph("FL.y.actual", LineColor<4>());
    _plot_eePosG_G->AddGraph("FL.z.actual", LineColor<5>());
    _plot_eePosG_G->AddGraph("BL.x.desired", LineColor<6>());
    _plot_eePosG_G->AddGraph("BL.y.desired", LineColor<7>());
    _plot_eePosG_G->AddGraph("BL.z.desired", LineColor<8>());
    _plot_eePosG_G->AddGraph("BL.x.actual", LineColor<9>());
    _plot_eePosG_G->AddGraph("BL.y.actual", LineColor<10>());
    _plot_eePosG_G->AddGraph("BL.z.actual", LineColor<11>());
    _plot_eePosG_G->AddGraph("FR.x.desired", LineColor<12>());
    _plot_eePosG_G->AddGraph("FR.y.desired", LineColor<13>());
    _plot_eePosG_G->AddGraph("FR.z.desired", LineColor<14>());
    _plot_eePosG_G->AddGraph("FR.x.actual", LineColor<15>());
    _plot_eePosG_G->AddGraph("FR.y.actual", LineColor<16>());
    _plot_eePosG_G->AddGraph("FR.z.actual", LineColor<17>());
    _plot_eePosG_G->AddGraph("BR.x.desired", LineColor<18>());
    _plot_eePosG_G->AddGraph("BR.y.desired", LineColor<19>());
    _plot_eePosG_G->AddGraph("BR.z.desired", LineColor<20>());
    _plot_eePosG_G->AddGraph("BR.x.actual", LineColor<21>());
    _plot_eePosG_G->AddGraph("BR.y.actual", LineColor<22>());
    _plot_eePosG_G->AddGraph("BR.z.actual", LineColor<23>());
    _plot_eePosG_G->show();
    RegisterPlot(_plot_eePosG_G.get());
#endif

#ifdef ENABLE_EEVELE_G_PLOT
    // _plot_velctrl2com = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_eeVelE_G->SetWindowTitle("eeVelE_G");
    _plot_eeVelE_G->AddGraph("FL.x.desired", LineColor<0>());
    _plot_eeVelE_G->AddGraph("FL.y.desired", LineColor<1>());
    _plot_eeVelE_G->AddGraph("FL.z.desired", LineColor<2>());
    _plot_eeVelE_G->AddGraph("FL.x.actual", LineColor<3>());
    _plot_eeVelE_G->AddGraph("FL.y.actual", LineColor<4>());
    _plot_eeVelE_G->AddGraph("FL.z.actual", LineColor<5>());
    _plot_eeVelE_G->AddGraph("BL.x.desired", LineColor<6>());
    _plot_eeVelE_G->AddGraph("BL.y.desired", LineColor<7>());
    _plot_eeVelE_G->AddGraph("BL.z.desired", LineColor<8>());
    _plot_eeVelE_G->AddGraph("BL.x.actual", LineColor<9>());
    _plot_eeVelE_G->AddGraph("BL.y.actual", LineColor<10>());
    _plot_eeVelE_G->AddGraph("BL.z.actual", LineColor<11>());
    _plot_eeVelE_G->AddGraph("FR.x.desired", LineColor<12>());
    _plot_eeVelE_G->AddGraph("FR.y.desired", LineColor<13>());
    _plot_eeVelE_G->AddGraph("FR.z.desired", LineColor<14>());
    _plot_eeVelE_G->AddGraph("FR.x.actual", LineColor<15>());
    _plot_eeVelE_G->AddGraph("FR.y.actual", LineColor<16>());
    _plot_eeVelE_G->AddGraph("FR.z.actual", LineColor<17>());
    _plot_eeVelE_G->AddGraph("BR.x.desired", LineColor<18>());
    _plot_eeVelE_G->AddGraph("BR.y.desired", LineColor<19>());
    _plot_eeVelE_G->AddGraph("BR.z.desired", LineColor<20>());
    _plot_eeVelE_G->AddGraph("BR.x.actual", LineColor<21>());
    _plot_eeVelE_G->AddGraph("BR.y.actual", LineColor<22>());
    _plot_eeVelE_G->AddGraph("BR.z.actual", LineColor<23>());
    _plot_eeVelE_G->show();
    RegisterPlot(_plot_eeVelE_G.get());
#endif

#ifdef ENABLE_THREAD_STATE_PLOT
    // _plot_threadState = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_threadState->SetWindowTitle("Thread state");
    _plot_threadState->AddGraph("Control thread period(ms)", LineColor<0>());
    _plot_threadState->AddGraph("Control thread load(ms)", LineColor<1>());
    _plot_threadState->AddGraph("MPC thread period(ms)", LineColor<2>());
    _plot_threadState->AddGraph("MPC thread load(ms)", LineColor<3>());
    _plot_threadState->AddGraph("IMU thread period(ms)", LineColor<4>());
    _plot_threadState->AddGraph("IMU thread load(ms)", LineColor<5>());
    _plot_threadState->AddGraph("Joystick thread period(ms)", LineColor<6>());
    _plot_threadState->AddGraph("Joystick thread load(ms)", LineColor<7>());
    _plot_threadState->show();
    RegisterPlot(_plot_threadState.get());
#endif

#ifdef ENABLE_DEBUG_DATA_PLOT
    //_plot_debugData = std::make_unique<PlotWindow>(_plotToolbox);
    _plot_debugData->SetWindowTitle("Debug Data Array");
    for (int gi = 0; gi < _debug_data_array_size_max; gi++)
    {
        _plot_debugData->AddGraph(QString("Data_%1").arg(gi, 2, 10, QLatin1Char('0')), LineColor(gi));
    }
    _plot_debugData->show();
    RegisterPlot(_plot_debugData.get());
#endif

    // 데이터 연결
#ifdef USE_TRANSPORT_ECAL
    _sub_state = std::make_unique<eCAL::protobuf::CSubscriber<dtproto::leoquad::LeoQuadStateTimeStamped>>("RobotState");
    _sub_state->AddReceiveCallback(std::bind(
        &LeoQuadDataHandler::OnRecvLeoQuadStateTimeStamped, this,
        std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4));
#endif
#ifdef USE_TRANSPORT_GRPC
    std::string ip;
    uint16_t port;
    GetServerAddress(ip, port);
    if (ip.empty() || port == 0)
    {
        ip = "127.0.0.1";
        port = 50051;
    }
    std::string svr_address = string_format("%s:%d", ip.c_str(), port);

    _sub_state = std::make_unique<dtCore::dtStateSubscriberGrpc<dtproto::leoquad::LeoQuadStateTimeStamped>>("RobotState", svr_address);
    std::function<void(dtproto::leoquad::LeoQuadStateTimeStamped &)> handler = [this](dtproto::leoquad::LeoQuadStateTimeStamped &msg)
    {
        static long long seq = 0;
        this->OnRecvLeoQuadStateTimeStamped("", msg, 0, seq++);
    };
    _sub_state->RegMessageHandler(handler);

    _sub_reconnector_running = true;
    _sub_reconnector = std::thread([this]
                                   {

        while (this->_sub_reconnector_running) {
            if (!this->_sub_state->IsRunning()) {
                LOG(warn) << "Disconnected. Reconnecting to LeoQuadState data server...";
                this->_sub_state->Reconnect();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } });
#endif
}

void LeoQuadDataHandler::OnRecvLeoQuadStateTimeStamped(const char *topic_name,
                                                       const dtproto::leoquad::LeoQuadStateTimeStamped &state,
                                                       const long long time, const long long clock)
{
    double curTime = clock * 1e-3;
    OnRecvLeoQuadState(curTime, state.state());

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

void LeoQuadDataHandler::OnRecvLeoQuadState(const double curTime, const dtproto::leoquad::LeoQuadState &state)
{
    OnRecvCpgState(curTime, state.cpgstate());
    OnRecvControlState(curTime, state.actcontrolstate(), state.descontrolstate());
    OnRecvJointState(curTime, state.jointstate(), state.actjointdata(), state.desjointdata());
    OnRecvThreadState(curTime, state.threadstate());
    OnRecvArbitraryState(curTime, state.arbitrarystate());
}

void LeoQuadDataHandler::OnRecvCpgState(const double curTime, const dtproto::leoquad::CpgState &state)
{
    if (_plot_cpgPhi)
    {
        _plot_cpgPhi->AddData(0, curTime, state.phi().a1());
        _plot_cpgPhi->AddData(1, curTime, state.phi().a2());
        _plot_cpgPhi->AddData(2, curTime, state.phi().a3());
        _plot_cpgPhi->AddData(3, curTime, state.phi().a4());
        _plot_cpgPhi->DataUpdated(curTime);
    }
    if (_plot_cpgCpg)
    {
        _plot_cpgCpg->AddData(0, curTime, state.cpg().a1());
        _plot_cpgCpg->AddData(1, curTime, state.cpg().a2());
        _plot_cpgCpg->AddData(2, curTime, state.cpg().a3());
        _plot_cpgCpg->AddData(3, curTime, state.cpg().a4());
        _plot_cpgCpg->DataUpdated(curTime);
    }
}

void LeoQuadDataHandler::OnRecvControlState(const double curTime, const dtproto::leoquad::ControlState &actState, const dtproto::leoquad::ControlState &desState)
{
    if (_plot_footForce)
    {
        for (int li = 0; li < legnum; li++)
        {
            _plot_footForce->AddData(3 * li + 0, curTime,
                                     desState.forceworld2footwrtworld(li).x());
            _plot_footForce->AddData(3 * li + 1, curTime,
                                     desState.forceworld2footwrtworld(li).y());
            _plot_footForce->AddData(3 * li + 2, curTime,
                                     desState.forceworld2footwrtworld(li).z());
        }
        _plot_footForce->DataUpdated(curTime);
    }
    if (_plot_footPos)
    {
        for (int li = 0; li < legnum; li++)
        {
            _plot_footPos->AddData(6 * li + 0, curTime,
                                   desState.posbody2footwrtbody(li).x());
            _plot_footPos->AddData(6 * li + 1, curTime,
                                   desState.posbody2footwrtbody(li).y());
            _plot_footPos->AddData(6 * li + 2, curTime,
                                   desState.posbody2footwrtbody(li).z());

            _plot_footPos->AddData(6 * li + 3, curTime,
                                   actState.posbody2footwrtbody(li).x());
            _plot_footPos->AddData(6 * li + 4, curTime,
                                   actState.posbody2footwrtbody(li).y());
            _plot_footPos->AddData(6 * li + 5, curTime,
                                   actState.posbody2footwrtbody(li).z());
        }
        _plot_footPos->DataUpdated(curTime);
    }
    if (_plot_footVel)
    {
        for (int li = 0; li < legnum; li++)
        {
            _plot_footVel->AddData(6 * li + 0, curTime,
                                   desState.velbody2footwrtbody(li).x());
            _plot_footVel->AddData(6 * li + 1, curTime,
                                   desState.velbody2footwrtbody(li).y());
            _plot_footVel->AddData(6 * li + 2, curTime,
                                   desState.velbody2footwrtbody(li).z());

            _plot_footVel->AddData(6 * li + 3, curTime,
                                   actState.velbody2footwrtbody(li).x());
            _plot_footVel->AddData(6 * li + 4, curTime,
                                   actState.velbody2footwrtbody(li).y());
            _plot_footVel->AddData(6 * li + 5, curTime,
                                   actState.velbody2footwrtbody(li).z());
        }
        _plot_footVel->DataUpdated(curTime);
    }
    if (_plot_comPos)
    {
        _plot_comPos->AddData(0, curTime,
                              desState.posworld2comwrtworld().x());
        _plot_comPos->AddData(1, curTime,
                              desState.posworld2comwrtworld().y());
        _plot_comPos->AddData(2, curTime,
                              desState.posworld2comwrtworld().z());
        _plot_comPos->AddData(3, curTime,
                              actState.posworld2comwrtworld().x());
        _plot_comPos->AddData(4, curTime,
                              actState.posworld2comwrtworld().y());
        _plot_comPos->AddData(5, curTime,
                              actState.posworld2comwrtworld().z());
        _plot_comPos->DataUpdated(curTime);
    }
    if (_plot_comVel)
    {
        _plot_comVel->AddData(0, curTime,
                              desState.velworld2comwrtworld().x());
        _plot_comVel->AddData(1, curTime,
                              desState.velworld2comwrtworld().y());
        _plot_comVel->AddData(2, curTime,
                              desState.velworld2comwrtworld().z());
        _plot_comVel->AddData(3, curTime,
                              actState.velworld2comwrtworld().x());
        _plot_comVel->AddData(4, curTime,
                              actState.velworld2comwrtworld().y());
        _plot_comVel->AddData(5, curTime,
                              actState.velworld2comwrtworld().z());
        _plot_comVel->DataUpdated(curTime);
    }
    if (_plot_eePosG_G)
    {
        for (int li = 0; li < legnum; li++)
        {
            _plot_eePosG_G->AddData(6 * li + 0, curTime,
                                    desState.posworld2footwrtworld(li).x());
            _plot_eePosG_G->AddData(6 * li + 1, curTime,
                                    desState.posworld2footwrtworld(li).y());
            _plot_eePosG_G->AddData(6 * li + 2, curTime,
                                    desState.posworld2footwrtworld(li).z());

            _plot_eePosG_G->AddData(6 * li + 3, curTime,
                                    actState.posworld2footwrtworld(li).x());
            _plot_eePosG_G->AddData(6 * li + 4, curTime,
                                    actState.posworld2footwrtworld(li).y());
            _plot_eePosG_G->AddData(6 * li + 5, curTime,
                                    actState.posworld2footwrtworld(li).z());
        }
        _plot_eePosG_G->DataUpdated(curTime);
    }
    if (_plot_eeVelE_G)
    {
        for (int li = 0; li < legnum; li++)
        {
            _plot_eeVelE_G->AddData(6 * li + 0, curTime,
                                    desState.velworld2footwrtworld(li).x());
            _plot_eeVelE_G->AddData(6 * li + 1, curTime,
                                    desState.velworld2footwrtworld(li).y());
            _plot_eeVelE_G->AddData(6 * li + 2, curTime,
                                    desState.velworld2footwrtworld(li).z());

            _plot_eeVelE_G->AddData(6 * li + 3, curTime,
                                    actState.velworld2footwrtworld(li).x());
            _plot_eeVelE_G->AddData(6 * li + 4, curTime,
                                    actState.velworld2footwrtworld(li).y());
            _plot_eeVelE_G->AddData(6 * li + 5, curTime,
                                    actState.velworld2footwrtworld(li).z());
        }
        _plot_eeVelE_G->DataUpdated(curTime);
    }
    if (_plot_orient)
    {
        _plot_orient->AddData(0, curTime,
                              desState.euleranglebodywrtworld().r() * RAD2DEG);
        _plot_orient->AddData(1, curTime,
                              desState.euleranglebodywrtworld().p() * RAD2DEG);
        _plot_orient->AddData(2, curTime,
                              desState.euleranglebodywrtworld().y() * RAD2DEG);
        _plot_orient->AddData(3, curTime,
                              actState.euleranglebodywrtworld().r() * RAD2DEG);
        _plot_orient->AddData(4, curTime,
                              actState.euleranglebodywrtworld().p() * RAD2DEG);
        _plot_orient->AddData(5, curTime,
                              actState.euleranglebodywrtworld().y() * RAD2DEG);
        _plot_orient->DataUpdated(curTime);
    }
    if (_plot_angVel)
    {
        _plot_angVel->AddData(0, curTime,
                              desState.angularvelbodywrtbody().r() * RAD2DEG);
        _plot_angVel->AddData(1, curTime,
                              desState.angularvelbodywrtbody().p() * RAD2DEG);
        _plot_angVel->AddData(2, curTime,
                              desState.angularvelbodywrtbody().y() * RAD2DEG);
        _plot_angVel->AddData(3, curTime,
                              actState.angularvelbodywrtbody().r() * RAD2DEG);
        _plot_angVel->AddData(4, curTime,
                              actState.angularvelbodywrtbody().p() * RAD2DEG);
        _plot_angVel->AddData(5, curTime,
                              actState.angularvelbodywrtbody().y() * RAD2DEG);
        _plot_angVel->DataUpdated(curTime);
    }
}

void LeoQuadDataHandler::OnRecvJointState(const double curTime,
                                          const google::protobuf::RepeatedPtrField<dtproto::leoquad::JointState> &state,
                                          const google::protobuf::RepeatedPtrField<dtproto::leoquad::JointData> &actData,
                                          const google::protobuf::RepeatedPtrField<dtproto::leoquad::JointData> &desData)
{
    if (_plot_jointPos)
    {
        for (int ji = 0; ji < jdof; ji++)
        {
            _plot_jointPos->AddData(
                2 * ji, curTime, desData.Get(ji).pos_rad());
            _plot_jointPos->AddData(
                2 * ji + 1, curTime, actData.Get(ji).pos_rad());
        }
        _plot_jointPos->DataUpdated(curTime);
    }
    if (_plot_jointVel)
    {
        for (int ji = 0; ji < jdof; ji++)
        {
            _plot_jointVel->AddData(
                2 * ji, curTime, desData.Get(ji).vel_rps());
            _plot_jointVel->AddData(
                2 * ji + 1, curTime, actData.Get(ji).vel_rps());
        }
        _plot_jointVel->DataUpdated(curTime);
    }
    if (_plot_jointAcc)
    {
        for (int ji = 0; ji < jdof; ji++)
        {
            _plot_jointAcc->AddData(
                2 * ji, curTime, desData.Get(ji).acc_rpss());
            _plot_jointAcc->AddData(
                2 * ji + 1, curTime, actData.Get(ji).acc_rpss());
        }
        _plot_jointAcc->DataUpdated(curTime);
    }
    if (_plot_jointTau)
    {
        for (int ji = 0; ji < jdof; ji++)
        {
            _plot_jointTau->AddData(
                2 * ji, curTime, desData.Get(ji).torq_nm());
            _plot_jointTau->AddData(
                2 * ji + 1, curTime, actData.Get(ji).torq_nm());
        }
        _plot_jointTau->DataUpdated(curTime);
    }
    if (_plot_absEnc)
    {
        for (int ji = 0; ji < jdof; ji++)
        {
            _plot_absEnc->AddData(ji, curTime, state.Get(ji).abspos_cnt());
        }
        _plot_absEnc->DataUpdated(curTime);
    }
    if (_plot_incEnc)
    {
        for (int ji = 0; ji < jdof; ji++)
        {
            _plot_incEnc->AddData(ji, curTime, state.Get(ji).incpos_cnt());
        }
        _plot_incEnc->DataUpdated(curTime);
    }
}

void LeoQuadDataHandler::OnRecvThreadState(const double curTime, const dtproto::leoquad::ThreadState &state)
{
    if (_plot_threadState)
    {
        _plot_threadState->AddData(0, curTime, state.ctrlperiod_ms());
        _plot_threadState->AddData(1, curTime, state.ctrlload_ms());
        _plot_threadState->AddData(2, curTime, state.mpcperiod_ms());
        _plot_threadState->AddData(3, curTime, state.mpcload_ms());
        _plot_threadState->AddData(4, curTime, state.imuperiod_ms());
        _plot_threadState->AddData(5, curTime, state.imuload_ms());
        _plot_threadState->AddData(6, curTime, state.joystickperiod_ms());
        _plot_threadState->AddData(7, curTime, state.joystickload_ms());
        _plot_threadState->DataUpdated(curTime);
    }
}

void LeoQuadDataHandler::OnRecvArbitraryState(const double curTime, const dtproto::std_msgs::PackedDouble &state)
{
    if (!_plot_debugData || state.GetTypeName() != "dtproto.std_msgs.PackedDouble")
    {
        return;
    }

    if (_debug_data_array_size < 0)
    {
        _debug_data_array_size = state.data_size();
        if (_debug_data_array_size > _debug_data_array_size_max)
            _debug_data_array_size = _debug_data_array_size_max;

        // data handler는 GUI thread와 다른 thread --> runtime warning 발생한다.
        // for (int gi = 0; gi < _debug_data_array_size; gi++) {
        //     _plot_debugData->AddGraph(QString("Data_%1.abs").arg(gi, 2, 10, QLatin1Char('0')), LineColor(gi));
        // }
    }

    for (int gi = 0; gi < _debug_data_array_size; gi++)
    {
        if (gi >= _debug_data_array_size_max || gi >= _plot_debugData->GetGraphCount())
            return;

        // LOG(trace) << "  data[" << gi << "] = " << state.data(gi);
        _plot_debugData->AddData(gi, curTime, state.data().Get(gi));
        _plot_debugData->DataUpdated(curTime);
    }
}
