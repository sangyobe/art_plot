#include "mainwindow.h"
#include "plotwindow.h"
#include <QApplication>
#include <ecal/ecal.h>

int main(int argc, char *argv[])
{
    eCAL::Initialize(0, nullptr, "art_plot::QuadIP::ControlStatus");
    eCAL::Process::SetState(proc_sev_healthy, proc_sev_level1, "proc info");

    QApplication a(argc, argv);

//    MainWindow w;
//    w.show();

    PlotWindow w1;
    w1.show();

//    PlotWindow w2;
//    w2.show();

    return a.exec();
}
