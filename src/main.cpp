#include "config.h"
#include "mainwindow.h"
#include "pconstants.h"
#include "plotwindow.h"
#include "cmdopts.hpp"
#include <QApplication>
#include <QDebug>
#include <dtCore/src/dtLog/dtLog.h>

void Usage();

int main(int argc, char *argv[])
{
    // Parse default arguments
    std::string profile_name = "";

    if (argc < 2)
    {
#if defined(ROBOT_NAME)
        profile_name = ROBOT_NAME;
#endif
    }
    else
    {
        profile_name = std::string(argv[1]);
    }

    if (profile_name == "")
    {
        Usage();
        return -1;
    }

    // Initialize app
    int rtn = 0;
    dt::Log::Initialize("artplot", "logs/artplot.txt");
    dt::Log::SetLogLevel(dt::Log::LogLevel::info);

    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/artplot.png"));

    // Parse optiona arguments starting with "--"
    struct AppOptions
    {
        std::string profile_name{};
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

    // Create main window
    MainWindow plotToolbox(app_opts.ip_addr, (uint16_t)app_opts.port, (int)app_opts.debug_data_num);
    plotToolbox.setWindowTitle(APP_NAME_WITH_VERSION);
    plotToolbox.setWindowFlag(Qt::WindowStaysOnTopHint);
    plotToolbox.show();

    // Create plot windows
    if (!plotToolbox.CreatePlotWindows(profile_name))
    {
        Usage();
        rtn = -1;
        goto finish;
    }

    // Start main application(event-loop)
    rtn = app.exec();

finish:
    dt::Log::Terminate();
    return rtn;
}

void Usage()
{
    printf("artplot: real-time data plot.\n");
    printf("\n");
    printf("Usage: artplot DATA_PROFILE_NAME [OPTION]...\n");
    printf("\n");
    printf("Open real-time plot windows for DATA_PROFILE_NAME.\n");
    printf("  DATA_PROFILE_NAME         Data profile name.    \n");
    printf("                            Possible data handlers are \"leoquad\", \"leoquad_w\", \"dualarm\", \"motoraging\", \"quadip\", \"woly\", and \"g1\"\n");
    printf("\n");
    printf("Options:\n");
    printf("  --ip IP_ADDRESS           ip_adress or hostname of the server. (default:localhost)    \n");
    // printf("  --port PORT_NUMBER        listening port number of the server.    \n");
    printf("  --dnum DEBUGDATA_NUM      Number of debug data (length of debugdata array. (default:32) \n");
    printf("\n");
    printf("Examples:\n");
    printf("  artplot leoquad\n");
    printf("  artplot leoquad --ip localhost --dnum 64\n");
}