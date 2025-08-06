#pragma once
#include <QWidget>
#include <QVector>
#include <QPointF>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>

class GraphWidget : public QWidget {
    Q_OBJECT
public:
    explicit GraphWidget(QWidget* parent = nullptr);
    ~GraphWidget() override = default;

public slots:
    void setSeries(const QVector<QPointF>& points, const QString& title = QString());

private:
    QChart*        chart  = nullptr;
    QLineSeries*   series = nullptr;
    QDateTimeAxis* xAxis  = nullptr;
    QValueAxis*    yAxis  = nullptr;
    QChartView*    view   = nullptr;
};
