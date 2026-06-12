#ifndef DIAGNOSTICSETTINGS_H
#define DIAGNOSTICSETTINGS_H

#include <QString>

struct DiagnosticSettings
{
    bool restartPicoAsha = true;
    bool deleteBonds = false;

    bool bluetoothAddresses = true;
    bool deviceNames = true;
    bool manufacturerNames = true;
    bool modelNames = true;
    bool deviceVersions = true;

    bool serialPortNames = true;
    bool serialNumbers = true;
    bool systemLocations = true;
    bool applicationAndOutputPaths = true;
    bool operatingSystemInformation = true;

    bool guiLog = true;
    bool rawHciTraffic = true;

    QString problemDescription;
    QString outputDirectory;
};

#endif // DIAGNOSTICSETTINGS_H
