#include "config.h"
#include "mainwindow.h"
#include "pconstants.h"
#include "plotwindow.h"
#include "cmdopts.hpp"
#include <QApplication>
#include <QDebug>
#include <dtCore/src/dtLog/dtLog.h>

#include "ArbitraryStateDataHandler.h"
#include "DualArmDataHandler.h"
#include "LeoQuadDataHandler.h"
#include "QuadIPDataHandler.h"
#include "WolyDataHandler.h"

int main(int argc, char *argv[])
{
    dt::Log::Initialize("artplot", "logs/artplot.txt");
    dt::Log::SetLogLevel(dt::Log::LogLevel::trace);

    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/artplot.png"));

    struct AppOptions
    {
        std::string ip_addr{};
        int port{};
        int debug_data_num{};
    };
    auto parser = CmdOpts<AppOptions>::Create({
        { "--ip",   &AppOptions::ip_addr },
        { "--port", &AppOptions::port },
        { "--dnum", &AppOptions::debug_data_num }
    });
    auto app_opts = parser->parse(argc, argv);

    MainWindow plotToolbox(app_opts.ip_addr, (uint16_t)app_opts.port, (int)app_opts.debug_data_num);
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
#ifdef ROBOT_DUALARM
    std::unique_ptr<DualArmDataHandler> dataHandler = std::make_unique<DualArmDataHandler>(&plotToolbox);
#endif

    // // special data source
    // std::unique_ptr<ArbitraryStateDataHandler> dataHandler_darray = std::make_unique<ArbitraryStateDataHandler>(&plotToolbox);

    // Start main application(event-loop)
    int rtn = app.exec();

finish:
    dt::Log::Terminate();
    return rtn;
}
