#include "define.h"
#include "mainwindow.h"
#include "pconstants.h"
#include "plotwindow.h"
#include <QApplication>
#include <QDebug>

#ifdef USE_TRANSPORT_ECAL
#include <ecal/ecal.h>
#endif

#include "QuadIPDataHandler.h"
#include "WolyDataHandler.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow plotToolbox;
    plotToolbox.setWindowTitle(APP_NAME);
    plotToolbox.setWindowFlag(Qt::WindowStaysOnTopHint);
    plotToolbox.show();

#ifdef USE_TRANSPORT_ECAL
    eCAL::Initialize(0, nullptr, "art_plot::QuadIP::ControlStatus");
    eCAL::Process::SetState(proc_sev_healthy, proc_sev_level1, "proc info");
#endif
#ifdef USE_TRANSPORT_GRPC
#endif

#ifdef ROBOT_QUADIP
    std::unique_ptr<QuadIPDataHandler> dataHandler = std::make_unique<QuadIPDataHandler>(&plotToolbox);
#endif
#ifdef ROBOT_WOLY
    std::unique_ptr<WolyDataHandler> dataHandler = std::make_unique<WolyDataHandler>(&plotToolbox);
#endif

    // Start main application(event-loop)
    return app.exec();
}
