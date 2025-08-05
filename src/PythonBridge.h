#pragma once
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QObject>
#include <iostream>

/* * PythonBridge.h
 * This class serves as a bridge between the Qt application and a Python script.
 * It uses QProcess to run the Python script and communicate with it via standard input/output.
 */


class PythonBridge : public QObject {
    Q_OBJECT
public:
    // Constructor that initializes the QProcess and connects signals
    explicit PythonBridge(QObject* parent = nullptr) : QObject(parent), proc(new QProcess(this)) {
        connect(proc, &QProcess::readyReadStandardOutput, this, &PythonBridge::onStdout);
        connect(proc, &QProcess::readyReadStandardError, this, &PythonBridge::onStderr);
        connect(proc, &QProcess::errorOccurred, this, &PythonBridge::onError);
        connect(proc, QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),
                this, &PythonBridge::onFinished);
    }

    void requestPrices(const QStringList& tickers, const QString& pythonExe, const QString& scriptPath) {
        proc->start(pythonExe, {scriptPath}); // Start the Python script with the executable
        if (!proc->waitForStarted(3000)) {
            emit failed("Python process failed to start!");
            std::cout << "proc->waitForStarted() failed to start\n";
            return;
        }
        QJsonObject req; // Tickers
        req["tickers"] = QJsonArray::fromStringList(tickers);
        auto bytes = QJsonDocument(req).toJson(QJsonDocument::Compact) + "\n";
        std::cout << bytes.toStdString() << "Making Request\n";
        proc->write(bytes);
        proc->closeWriteChannel();
    }

signals:
    void received(const QJsonObject& payload);
    void failed(const QString& msg);

private slots:
    void onStdout() { // Read and process stdout
        buffer += proc->readAllStandardOutput();
        int idx;
        while ((idx = buffer.indexOf('\n')) != -1) { // Process each line
            QByteArray line = buffer.left(idx);
            buffer.remove(0, idx + 1);
            auto doc = QJsonDocument::fromJson(line);
            if (doc.isObject()) emit received(doc.object());
        }
    }
    void onStderr() {
        const QByteArray err = proc->readAllStandardError();
        qWarning().noquote() << "[pystderr]" << QString::fromUtf8(err);
    }
    void onError(QProcess::ProcessError error) {
        emit failed(QString("QProcess error %1").arg(int(error)));
    }
    void onFinished(int, QProcess::ExitStatus) {} // No action needed on finish

private:
    QProcess* proc;
    QByteArray buffer;
};
