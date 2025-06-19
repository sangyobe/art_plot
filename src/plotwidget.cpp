#include "plotwidget.h"
#include "pconstants.h"
#include "plotwindow.h"
#include <QDebug>

#define MCAP_COMPRESSION_NO_LZ4
#define MCAP_COMPRESSION_NO_ZSTD
#define MCAP_IMPLEMENTATION // Define this in exactly one .cpp file
#include <google/protobuf/descriptor.h>
#include <mcap/mcap.hpp>

PlotWidget::PlotWidget(QWidget *parent) : QCustomPlot(parent)
{
    // connect(this, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(ShowPointToolTip(QMouseEvent*)));
    connect(this, &QCustomPlot::mouseMove, this, &PlotWidget::ShowPointToolTip);
}

PlotWidget::~PlotWidget()
{
}

void PlotWidget::ExportData(QString &export_dir)
{
    QDateTime date = QDateTime::currentDateTime();
    QString formattedTime = date.toString("yyyyMMdd_hhmmss");

    for (auto graph : const_cast<const QList<QCPGraph*> &>(mGraphs))
    {
        QString filename = export_dir + "/" + formattedTime + "_" + windowTitle() + "_" + graph->name() + ".csv";
        // qDebug() << "export file: " << filename;

        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly | QFile::Truncate))
        {
            qDebug() << "Cannot open file, " << filename << " , for exporting.";
            qDebug() << file.errorString();
            continue;
        }

        QTextStream out(&file);

        out << "_time_"
            << ","
            << graph->name() << "|"
            << graph->lineStyle() << "|"
            << graph->pen().color().rgb() << "|"
            << (graph->lineStyle() == QCPGraph::lsNone ? 0 : graph->pen().width()) << "|"
            << graph->scatterStyle().shape() << "|"
            << graph->scatterStyle().size() << "|"
            << graph->scatterSkip() << "\n";
        std::for_each(graph->data()->constBegin(), graph->data()->constEnd(), [&out](QCPGraphData data)
                      { out << data.key << "," << data.value << "\n"; });

        file.flush();
        file.close();
    }
}

void PlotWidget::ImportCSV(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot open file, " << filename << " , for importing.";
        qDebug() << file.errorString();
        return;
    }

    int parse_header = 0;
    QList<QCPGraph *> graphs;
    int graph_base_idx = -1;
    int graph_header_item_count = 9;
    int graph_count = 0;

    int xidx = -1; // x-axis index (-1: no x-axis data)
    int xval = 0;
    int hidx = 0; // header item index

    while (!file.atEnd())
    {
        QByteArray line = file.readLine();
        QList<QByteArray> elements = line.split(',');

        // 앞/뒤 따옴표 있으면 따옴표 제거
        std::for_each(elements.begin(), elements.end(), [](QByteArray &element) {
            if (element.startsWith("\""))
                element = element.right(element.size()-1);
            if (element.endsWith('\"'))
                element = element.left(element.size()-1);
            if (element.endsWith('\n'))
                element = element.left(element.size()-1); });

        if (elements.size() <= 0 || elements[0].size() == 0)
            continue;

        if (parse_header == 0)
        {

            if (elements.size() > 1 && elements[0] == "_time_")
            {
                xidx = 0;
                xval = 0;
                hidx = 1; // skip x-axis
            }
            else
            {
                xidx = -1;
                hidx = 0;
            }

            while (elements.size() > hidx)
            {
                QString name = elements[hidx].toStdString().c_str();
                if (name.isEmpty())
                { // empty data name
                    // break;
                    name = "data_" + QString::number(hidx);
                }

                QStringList mdata = name.split('|');
                name = mdata[0];
                int line_style = 1;
                QColor line_color = LineColor((xidx == 0 ? hidx - 1 : hidx));
                int line_width = 1;
                int scatter_shape = 0;
                double scatter_size = 6;
                int scatter_skip = 0;

                if (mdata.size() > 1)
                    line_style = mdata[1].toInt();
                if (mdata.size() > 2)
                    line_color.setRgb(mdata[2].toUInt());
                if (mdata.size() > 3)
                    line_width = mdata[3].toInt();
                if (mdata.size() > 4)
                    scatter_shape = mdata[4].toInt();
                if (mdata.size() > 5)
                    scatter_size = mdata[5].toDouble();
                if (mdata.size() > 6)
                    scatter_skip = mdata[6].toInt();

                if (line_style == QCPGraph::lsNone)
                    line_width = 0;

                int idx = static_cast<PlotWindow *>(parent()->parent())->AddGraph(name, line_color, "", line_width, scatter_shape, scatter_skip);
                if (graph_count == 0)
                    graph_base_idx = (idx - 1); // AddGraph returns graph count not index
                graph_count++;
                hidx++;
            }

            parse_header = 1;
            continue;
        }

        double key = (xidx == 0 ? elements[0].toDouble() : (double)(xval++));
        for (int gidx = 0; gidx < graph_count; gidx++)
        {
            int eidx = (xidx == 0 ? gidx + 1 : gidx);
            if (eidx >= elements.size())
                break; // skip processing this line
            mGraphs[graph_base_idx + gidx]->addData(key, elements[eidx].toDouble());
        }
    }

    file.close();
}

void PlotWidget::ImportMcap(QString filename)
{
    mcap::McapReader reader;
    mcap::Status status;

    status = reader.open(filename.toStdString());
    if (!status.ok())
    {
        qDebug() << "could not open mcap file(" << filename << ")";
        return;
    }

    status = reader.readSummary(mcap::ReadSummaryMethod::NoFallbackScan);
    if (!status.ok())
    {
        qDebug() << "could not read summary of a mcap file(" << filename << ")";
        return;
    }

    for (auto channel : reader.channels())
    {
        qDebug() << channel.second->topic.c_str() << " / " << reader.schema(channel.second->schemaId)->name.c_str() << " / " << reader.schema(channel.second->schemaId)->encoding.c_str();
    }

    for (auto schema : reader.schemas())
    {
        qDebug() << schema.second->name.c_str() << " / " << schema.second->encoding.c_str();
    }

    return;
}

void PlotWidget::ImportData(QStringList &import_files)
{
    for (QString &filename : import_files)
    {
        QFileInfo finfo(filename);
        if ("mcap" == finfo.suffix().toLower())
        {
            ImportMcap(filename);
        }
        else if ("csv" == finfo.suffix().toLower())
        {
            ImportCSV(filename);
        }
    }
}

void PlotWidget::ShowPointToolTip(QMouseEvent *event)
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    double x = this->xAxis->pixelToCoord(event->x());
    double y = this->yAxis->pixelToCoord(event->y());
#else
    double x = this->xAxis->pixelToCoord(event->position().x());
    double y = this->yAxis->pixelToCoord(event->position().y());
#endif
    setToolTip(QString("%1, %2").arg(QString::number(x, 'f')).arg(QString::number(y, 'f')));
}

QCPRange PlotWidget::getKeyRange(bool &foundRange)
{
    QCPRange range;
    foundRange = false;

    for (auto graph : const_cast<const QList<QCPGraph*> &>(mGraphs))
    {
        QCPRange rng;
        bool fnd;
        rng = graph->getKeyRange(fnd);
        if (fnd)
        {
            if (!foundRange)
            {
                range = rng;
                foundRange = true;
            }
            else
                range.expand(rng);
        }
    }

    return range;
}

QCPRange PlotWidget::getValueRange(bool &foundRange)
{
    QCPRange range;
    foundRange = false;

    for (auto graph : const_cast<const QList<QCPGraph*> &>(mGraphs))
    {
        QCPRange rng;
        bool fnd;
        rng = graph->getValueRange(fnd);
        if (fnd)
        {
            if (!foundRange)
            {
                range = rng;
                foundRange = true;
            }
            else
                range.expand(rng);
        }
    }

    return range;
}
