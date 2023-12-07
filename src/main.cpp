#include "define.h"
#include "mainwindow.h"
#include "pconstants.h"
#include "plotwindow.h"
#include "cmdopts.hpp"
#include <QApplication>
#include <QDebug>

#ifdef USE_TRANSPORT_ECAL
#include <ecal/ecal.h>
#endif

#include "QuadIPDataHandler.h"
#include "WolyDataHandler.h"
#include "LeoQuadDataHandler.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    struct AppOptions
    {
        std::string ip_addr{};
        int port{};
    };
    auto parser = CmdOpts<AppOptions>::Create({
        { "--ip",   &AppOptions::ip_addr },
        { "--port", &AppOptions::port }
    });
    auto app_opts = parser->parse(argc, argv);

    MainWindow plotToolbox(app_opts.ip_addr, (uint16_t)app_opts.port);
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
#ifdef ROBOT_LEOQUAD
    std::unique_ptr<LeoQuadDataHandler> dataHandler = std::make_unique<LeoQuadDataHandler>(&plotToolbox);
#endif

    // Start main application(event-loop)
    int rtn = app.exec();

finish:
    return rtn;
}
