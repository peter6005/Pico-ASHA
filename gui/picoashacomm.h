#ifndef PICOASHACOMM_H
#define PICOASHACOMM_H

#include <QObject>
#include <QByteArray>
#include <QFile>
#include <QSerialPort>
#include <QString>
#include <QStringListModel>
#include <QTimer>

#include <asha_comms.hpp>
#include "diagnosticsettings.h"
#include "picoashamainwindow.h"

class PicoAshaComm : public QObject
{
    Q_OBJECT

public:
    explicit PicoAshaComm(QObject *parent = nullptr);
    virtual ~PicoAshaComm();

    void showUI() const;

    QString paFirmwareVers() const;
    void setPaFirmwareVers(const QString &newPaFirmwareVers);

    QString remoteError() const;
    void setRemoteError(const QString &newRemoteError);

    QString errMsg() const;
    void setErrMsg(const QString &newErrMsg);

private:
    void handleDecodedData(QByteArray const& decoded);
    void handleEventPacket(asha::comm::HeaderPacket const header, asha::comm::EventPacket const& pkt);
    bool checkError(asha::comm::HeaderPacket const header, asha::comm::EventPacket const& pkt, QString const& state);

    QString logHeader(asha::comm::HeaderPacket const header);
    bool appendLog(QString const& logLine);

    void closeSerial();

    bool sendCommandPacket(asha::comm::CmdPacket const& cmd_pkt);

    void writeHciPacket(const char* data, size_t len);
    bool startHciLogging(QString const& path);
    bool stopHciLogging();
    bool prepareDiagnosticOutput(QString const& path);
    void failDiagnosticStart(QString const& message);

    QSerialPort m_serial;
    QByteArray m_currPacket;

    QTimer connect_timer;
    QTimer intro_timer;

    PicoAshaMainWindow* m_ui;

    QFile m_hciLogFile;

    bool m_serialConnected;

    QString m_paFirmwareVers;

    QString m_remoteError;

    bool m_hciLoggingEnabled;

    bool m_diagnosticRecordingActive = false;
    DiagnosticSettings m_diagnosticSettings;

    QString m_errMsg;

signals:

    void paFirmwareVersChanged();

    void remoteErrorChanged();

    void errMsgChanged();
    void diagnosticSessionStartFailed(QString const& message);

public slots:
    void onConnectTimer();
    void onIntroTimer();
    void onSerialError(QSerialPort::SerialPortError error);
    void onSerialReadyRead();
    void onDiagnosticSessionStartRequested(DiagnosticSettings const& settings);
    void onDiagnosticSessionFinishRequested();
    void onCmdRestartBtnClicked();
    void onCmdConnAllowedBtnClicked(bool allowed);
    void onCmdStreamingEnabledBtnClicked(bool enabled);
    void onCmdRemoveBondBtnClicked();
    void onUsbSettingsBtnClicked(asha::comm::USBInfo const& usb_info);
    void onPairWithAddress(QByteArray const& addr, uint8_t addr_type);
};

#endif // PICOASHACOMM_H
