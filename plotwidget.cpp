#include "plotwidget.h"
#include "plotwindow.h"
#include "pconstants.h"
#include <QDebug>

PlotWidget::PlotWidget(QWidget *parent) :
  QCustomPlot(parent)
{
    //connect(this, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(ShowPointToolTip(QMouseEvent*)));
    connect(this, &QCustomPlot::mouseMove, this, &PlotWidget::ShowPointToolTip);
}

PlotWidget::~PlotWidget()
{
}

void PlotWidget::ExportToCSV(QString& export_dir)
{
    QDateTime date = QDateTime::currentDateTime();
    QString formattedTime = date.toString("yyyyMMdd_hhmmss");

    for (auto graph : qAsConst(mGraphs)) {
        QString filename = export_dir + "/" + formattedTime + "_" + windowTitle() + "_" + graph->name() + ".csv";
        qDebug() << "export file: " << filename;

        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly | QFile::Truncate)) {
            qDebug() << file.errorString();
            continue;
        }

        QTextStream out(&file);

        out << graph->name() << ","
            << graph->lineStyle() << ","
            << graph->pen().color().red() << ","
            << graph->pen().color().green() << ","
            << graph->pen().color().blue() << ","
            << graph->pen().width() << ","
            << graph->scatterStyle().shape() << ","
            << graph->scatterStyle().size() << ","
            << graph->scatterSkip() << "\n";
        std::for_each(graph->data()->constBegin(), graph->data()->constEnd(), [&out](QCPGraphData data) {
            out << data.key << "," << data.value << "\n";
        });

        file.flush();
        file.close();
    }
}

void PlotWidget::ImportFromCSV(QStringList& import_files)
{
    for (QString& filename : import_files) {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << file.errorString();
            continue;
        }

        int parse_header = 0;
        QCPGraph* graph = nullptr;

        while (!file.atEnd()) {
            QByteArray line = file.readLine();
            QList<QByteArray> elements = line.split(',');

            // 앞/뒤 따옴표 있으면 따옴표 제거
            std::for_each(elements.begin(), elements.end(), [](QByteArray& element) {
                if (element.startsWith("\""))
                    element = element.right(element.size()-1);
                if (element.endsWith('\"'))
                    element = element.left(element.size()-1);
            });

            if (parse_header == 0) {

                QString name = elements[0].toStdString().c_str();
                //int line_style = elements[1].toInt();
                QColor line_color(elements[2].toInt(), elements[3].toInt(), elements[4].toInt());
                int line_width = elements[5].toInt();
                int scatter_shape = elements[6].toInt();
                //double scatter_size = elements[7].toDouble();
                int scatter_skip = elements[8].toInt();

                int idx = static_cast<PlotWindow*>(parent()->parent())->AddGraph(name, line_color, line_width, scatter_shape, scatter_skip);
                graph = mGraphs[idx-1];

                parse_header = 1;
                continue;
            }

            double key = elements[0].toDouble();
            double val = elements[1].toDouble();
            graph->addData(key, val);

        }

        file.close();
    }
}

void PlotWidget::ShowPointToolTip(QMouseEvent *event)
{
    double x = this->xAxis->pixelToCoord(event->x());
    double y = this->yAxis->pixelToCoord(event->y());
    setToolTip(QString("%1, %2").arg(x).arg(y));
}

QCPRange PlotWidget::getKeyRange(bool &foundRange)
{
    QCPRange range;
    foundRange = false;

    for (auto graph : qAsConst(mGraphs)) {
        QCPRange rng;
        bool fnd;
        rng = graph->getKeyRange(fnd);
        if (fnd) {
            range.expand(rng);
            foundRange = true;
        }
    }

    return range;
}

QCPRange PlotWidget::getValueRange(bool &foundRange)
{
    QCPRange range;
    foundRange = false;

    for (auto graph : qAsConst(mGraphs)) {
        QCPRange rng;
        bool fnd;
        rng = graph->getValueRange(fnd);
        if (fnd) {
            range.expand(rng);
            foundRange = true;
        }
    }

    return range;
}
