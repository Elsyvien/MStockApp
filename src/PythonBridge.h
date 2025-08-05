#pragma once
#include <QProcess>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <cstdio>
#include <iostream>
#include <QJsonArray>

class PythonBridge : public QObject {
    Q_OBJECT
public:
    explicit PythonBridge(QObject* parent = nullptr) : QObject(parent), proc(new QProcess(this)) {
        connect(proc, &QProcess::readyReadStandardOutput, this, &PythonBridge::onStdout);
        connect(proc, &QProcess::readyReadStandardError, this, &PythonBridge::onStderr);
        connect(proc, &QProcess::errorOccurred, this, &PythonBridge::onError);
        connect(proc, QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),
                this, &PythonBridge::onFinished);
    }

    void requestPrices(const QStringList& tickers, const QString& pythonExe, const QString& scriptPath) {
        // Launch Process
        proc -> start(pythonExe, {scriptPath});
        if (!proc -> waitForStarted(3000)) {
            emit failed("Python process failed to start!");
            std::cout << "proc -> waitForStarted() failed to start" << std::endl; // For debugging!
            return;
        }
        // Send JSON Request to Python Script
        QJsonObject req;
        req["tickers"] = QJsonArray::fromStringList(tickers);
        auto bytes = QJsonDocument(req).toJson(QJsonDocument::Compact) + "\n";
        std::cout << bytes.toStdString() << "Making Request" << std::endl;
        proc -> write(bytes);
        proc -> closeWriteChannel();
    }
signals:
    void received(const QJsonObject& payload);
    void failed(const QString& msg);
private slots:
    void onStdout() {
        buffer += proc -> readAllStandardOutput();
        // Read each line
        int idx;
        while ((idx = buffer.indexOf("\n")) != -1) {
            QByteArray line = buffer.left(idx);
            buffer.remove(0, idx + 1);
            auto doc = QJsonDocument::fromJson(line);
            if (doc.isObject()) emit received(doc.object());
        }
        std::cout << buffer.toStdString() << std::endl;
    }
    void onError(QProcess::ProcessError error) {
        emit failed(QString("QProcess error %1").arg(int(error)));
    }
    void onFinished(int code, QProcess::ExitStatus exitStatus) {
        Q_UNUSED(code); Q_UNUSED(exitStatus);
    }
    void onStderr() {
        const QByteArray err = proc->readAllStandardError();
        qWarning().noquote() << "[pystderr]" << QString::fromUtf8(err);
}
private:
    QProcess* proc;
    QByteArray buffer;
};
