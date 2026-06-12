#ifndef DIAGNOSTICSESSIONDIALOG_H
#define DIAGNOSTICSESSIONDIALOG_H

#include <QDialog>
#include <QList>

#include "diagnosticsettings.h"

class QCheckBox;
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QToolButton;
class QVBoxLayout;
class QWidget;

class DiagnosticSessionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DiagnosticSessionDialog(DiagnosticSettings const& initialSettings = {},
                                     QWidget *parent = nullptr);

    DiagnosticSettings settings() const;

private:
    struct PrivacyGroup {
        QToolButton* expandButton;
        QCheckBox* groupCheckBox;
        QWidget* detailsWidget;
        QList<QCheckBox*> fields;
    };

    PrivacyGroup addPrivacyGroup(QVBoxLayout* parentLayout,
                                 QString const& title,
                                 QStringList const& fieldNames);
    void updateGroupCheckState(PrivacyGroup const& group);
    void updateStartButton();
    void chooseOutputDirectory();

    QCheckBox* m_restartCheckBox;
    QCheckBox* m_deleteBondsCheckBox;

    PrivacyGroup m_bluetoothGroup;
    PrivacyGroup m_computerGroup;
    QCheckBox* m_guiLogCheckBox;
    QCheckBox* m_rawHciCheckBox;

    QPlainTextEdit* m_problemDescriptionEdit;
    QLineEdit* m_outputDirectoryEdit;
    QLabel* m_validationLabel;
    QDialogButtonBox* m_buttonBox;
};

#endif // DIAGNOSTICSESSIONDIALOG_H
