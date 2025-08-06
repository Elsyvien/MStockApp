#include "GraphWidget.h"
#include <QVBoxLayout>
#include <QDateTime>

GraphWidget::GraphWidget(QWidget* parent): QWidget(parent),
      chart(new QChart),
      series(new QLineSeries),
      xAxis(new QDateTimeAxis),
      yAxis(new QValueAxis),
      view(new QChartView(chart))
{
    chart->addSeries(series);
    chart->legend()->hide();

    xAxis->setFormat("dd.MM\nHH:mm");
    xAxis->setTitleText("Zeit");
    yAxis->setTitleText("Preis");

    chart->addAxis(xAxis, Qt::AlignBottom);
    chart->addAxis(yAxis, Qt::AlignLeft);
    series->attachAxis(xAxis);
    series->attachAxis(yAxis);

    view->setRubberBand(QChartView::RectangleRubberBand);

    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(view);
    setLayout(lay);
}

void GraphWidget::setSeries(const QVector<QPointF>& points, const QString& title) {
    series->replace(points);
    if (!title.isEmpty())
        chart->setTitle(title);

    if (!points.isEmpty()) {
        const qreal minX = points.first().x();
        const qreal maxX = points.last().x();
        xAxis->setRange(QDateTime::fromMSecsSinceEpoch(minX),
                        QDateTime::fromMSecsSinceEpoch(maxX));

        qreal minY = points.first().y(), maxY = minY;
        for (const QPointF& p : points) {
            if (p.y() < minY) minY = p.y();
            if (p.y() > maxY) maxY = p.y();
        }
        const qreal pad = (maxY - minY) * 0.05;
        yAxis->setRange(minY - pad, maxY + pad);
    }
}
