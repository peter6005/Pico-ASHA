#include "diagnosticsessiondialog.h"

#include <QCheckBox>
#include <QDateTime>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QStandardPaths>
#include <QToolButton>
#include <QVBoxLayout>

namespace {
class PrivacyGroupCheckBox : public QCheckBox
{
public:
    using QCheckBox::QCheckBox;

protected:
    void nextCheckState() override
    {
        setCheckState(checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);
    }
};
} // namespace

DiagnosticSessionDialog::DiagnosticSessionDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Start Diagnostic Session");
    setMinimumWidth(560);
    resize(620, 740);

    auto mainLayout = new QVBoxLayout(this);

    auto scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    mainLayout->addWidget(scrollArea, 1);

    auto contentWidget = new QWidget;
    auto contentLayout = new QVBoxLayout(contentWidget);
    scrollArea->setWidget(contentWidget);

    auto problemLabel = new QLabel("Problem description (required)");
    problemLabel->setStyleSheet("font-weight: bold");
    contentLayout->addWidget(problemLabel);

    m_problemDescriptionEdit = new QPlainTextEdit;
    m_problemDescriptionEdit->setPlaceholderText(
        "Briefly describe what went wrong and what you expected to happen, "
        "for example: \"left hearing aid never connects\" or \"audio stops after a few seconds\".");
    m_problemDescriptionEdit->setToolTip(
        "This text will be included in the diagnostic files. Review it before sharing.");
    m_problemDescriptionEdit->setFixedHeight(100);
    contentLayout->addWidget(m_problemDescriptionEdit);

    auto outputLabel = new QLabel("Save diagnostic files (required)");
    outputLabel->setStyleSheet("font-weight: bold");
    contentLayout->addWidget(outputLabel);

    auto outputLayout = new QHBoxLayout;
    m_outputDirectoryEdit = new QLineEdit;
    const QString documentsPath =
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    const QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss");
    m_outputDirectoryEdit->setText(
        QDir(documentsPath).filePath(QString("pico-asha-diagnostic-%1").arg(timestamp)));
    m_outputDirectoryEdit->setToolTip("The diagnostic files will be created in this folder.");
    outputLayout->addWidget(m_outputDirectoryEdit, 1);

    auto browseButton = new QPushButton("Browse...");
    browseButton->setToolTip("Choose the parent folder for the diagnostic files.");
    outputLayout->addWidget(browseButton);
    contentLayout->addLayout(outputLayout);

    auto privacyLabel = new QLabel("Privacy and included data");
    privacyLabel->setStyleSheet("font-weight: bold");
    contentLayout->addWidget(privacyLabel);

    auto privacyNotice = new QLabel(
        "All diagnostic data is selected by default. Turn off any item you do not want to include.");
    privacyNotice->setWordWrap(true);
    contentLayout->addWidget(privacyNotice);

    m_bluetoothGroup = addPrivacyGroup(
        contentLayout,
        "Bluetooth and hearing-aid information",
        {
            "Bluetooth addresses",
            "Device names",
            "Manufacturer names",
            "Model names",
            "Firmware and software versions"
        });

    m_computerGroup = addPrivacyGroup(
        contentLayout,
        "Computer and USB information",
        {
            "Serial port names",
            "Serial numbers",
            "System locations",
            "Application and output paths",
            "Operating system information"
        });

    m_bluetoothGroup.groupCheckBox->setChecked(true);
    m_computerGroup.groupCheckBox->setChecked(true);

    auto guiLogFrame = new QFrame;
    guiLogFrame->setFrameShape(QFrame::StyledPanel);
    auto guiLogLayout = new QVBoxLayout(guiLogFrame);
    m_guiLogCheckBox = new QCheckBox("GUI log");
    m_guiLogCheckBox->setChecked(true);
    m_guiLogCheckBox->setToolTip("Include the log shown in the main Pico-ASHA window.");
    guiLogLayout->addWidget(m_guiLogCheckBox);
    auto guiLogDescription = new QLabel("Includes the messages shown in the main window.");
    guiLogDescription->setWordWrap(true);
    guiLogLayout->addWidget(guiLogDescription);
    contentLayout->addWidget(guiLogFrame);

    auto hciFrame = new QFrame;
    hciFrame->setFrameShape(QFrame::StyledPanel);
    auto hciLayout = new QVBoxLayout(hciFrame);
    m_rawHciCheckBox = new QCheckBox("Raw HCI traffic");
    m_rawHciCheckBox->setChecked(true);
    m_rawHciCheckBox->setToolTip("Include a detailed record of Bluetooth communication.");
    hciLayout->addWidget(m_rawHciCheckBox);

    auto hciWarning = new QLabel(
        "Sensitive: records detailed Bluetooth communication and can reveal addresses and "
        "information about Pico-ASHA, hearing aids, and other nearby or connected Bluetooth "
        "devices. Turning off the choices above does not remove this information from raw HCI.");
    hciWarning->setWordWrap(true);
    hciLayout->addWidget(hciWarning);
    contentLayout->addWidget(hciFrame);

    auto actionsLabel = new QLabel("Before recording");
    actionsLabel->setStyleSheet("font-weight: bold");
    contentLayout->addWidget(actionsLabel);

    m_restartCheckBox = new QCheckBox("Restart Pico-ASHA (recommended)");
    m_restartCheckBox->setChecked(true);
    m_restartCheckBox->setToolTip("Restart Pico-ASHA before diagnostic recording starts.");
    contentLayout->addWidget(m_restartCheckBox);

    m_deleteBondsCheckBox = new QCheckBox("Delete saved bonds (strongly recommended for pairing problems)");
    m_deleteBondsCheckBox->setToolTip(
        "Remove saved hearing-aid bonds before recording. Strongly recommended when reproducing "
        "a pairing problem.");
    contentLayout->addWidget(m_deleteBondsCheckBox);
    contentLayout->addStretch();

    m_validationLabel = new QLabel;
    m_validationLabel->setWordWrap(true);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    m_buttonBox->button(QDialogButtonBox::Ok)->setText("Start");

    auto footerLayout = new QHBoxLayout;
    footerLayout->addWidget(m_validationLabel, 1);
    footerLayout->addWidget(m_buttonBox);
    mainLayout->addLayout(footerLayout);

    connect(browseButton, &QPushButton::clicked, this, &DiagnosticSessionDialog::chooseOutputDirectory);
    connect(m_problemDescriptionEdit, &QPlainTextEdit::textChanged, this, &DiagnosticSessionDialog::updateStartButton);
    connect(m_outputDirectoryEdit, &QLineEdit::textChanged, this, &DiagnosticSessionDialog::updateStartButton);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    updateStartButton();
}

DiagnosticSettings DiagnosticSessionDialog::settings() const
{
    DiagnosticSettings result;
    result.restartPicoAsha = m_restartCheckBox->isChecked();
    result.deleteBonds = m_deleteBondsCheckBox->isChecked();

    result.bluetoothAddresses = m_bluetoothGroup.fields.at(0)->isChecked();
    result.deviceNames = m_bluetoothGroup.fields.at(1)->isChecked();
    result.manufacturerNames = m_bluetoothGroup.fields.at(2)->isChecked();
    result.modelNames = m_bluetoothGroup.fields.at(3)->isChecked();
    result.deviceVersions = m_bluetoothGroup.fields.at(4)->isChecked();

    result.serialPortNames = m_computerGroup.fields.at(0)->isChecked();
    result.serialNumbers = m_computerGroup.fields.at(1)->isChecked();
    result.systemLocations = m_computerGroup.fields.at(2)->isChecked();
    result.applicationAndOutputPaths = m_computerGroup.fields.at(3)->isChecked();
    result.operatingSystemInformation = m_computerGroup.fields.at(4)->isChecked();

    result.guiLog = m_guiLogCheckBox->isChecked();
    result.rawHciTraffic = m_rawHciCheckBox->isChecked();
    result.problemDescription = m_problemDescriptionEdit->toPlainText().trimmed();
    result.outputDirectory = QDir::cleanPath(m_outputDirectoryEdit->text().trimmed());
    return result;
}

DiagnosticSessionDialog::PrivacyGroup DiagnosticSessionDialog::addPrivacyGroup(
    QVBoxLayout* parentLayout,
    const QString &title,
    const QStringList &fieldNames)
{
    auto frame = new QFrame;
    frame->setFrameShape(QFrame::StyledPanel);
    auto frameLayout = new QVBoxLayout(frame);

    auto headerLayout = new QHBoxLayout;
    auto expandButton = new QToolButton;
    expandButton->setArrowType(Qt::RightArrow);
    expandButton->setCheckable(true);
    expandButton->setToolTip("Show or hide individual privacy settings.");
    headerLayout->addWidget(expandButton);

    auto groupCheckBox = new PrivacyGroupCheckBox(title);
    groupCheckBox->setTristate(true);
    headerLayout->addWidget(groupCheckBox, 1);
    frameLayout->addLayout(headerLayout);

    auto detailsWidget = new QWidget;
    auto detailsLayout = new QVBoxLayout(detailsWidget);
    detailsLayout->setContentsMargins(32, 0, 0, 0);

    QList<QCheckBox*> fields;
    for (const auto& fieldName : fieldNames) {
        auto field = new QCheckBox(fieldName);
        fields.append(field);
        detailsLayout->addWidget(field);
    }
    detailsWidget->setVisible(false);
    frameLayout->addWidget(detailsWidget);
    parentLayout->addWidget(frame);

    PrivacyGroup group{
        .expandButton = expandButton,
        .groupCheckBox = groupCheckBox,
        .detailsWidget = detailsWidget,
        .fields = fields
    };

    connect(expandButton, &QToolButton::toggled, this, [group](bool expanded) {
        group.expandButton->setArrowType(expanded ? Qt::DownArrow : Qt::RightArrow);
        group.detailsWidget->setVisible(expanded);
    });

    connect(groupCheckBox, &QCheckBox::checkStateChanged, this, [this, group](Qt::CheckState state) {
        if (state == Qt::PartiallyChecked) {
            return;
        }
        for (auto field : group.fields) {
            field->blockSignals(true);
            field->setChecked(state == Qt::Checked);
            field->blockSignals(false);
        }
        updateGroupCheckState(group);
    });

    for (auto field : fields) {
        connect(field, &QCheckBox::checkStateChanged, this, [this, group](Qt::CheckState) {
            updateGroupCheckState(group);
        });
    }

    updateGroupCheckState(group);
    return group;
}

void DiagnosticSessionDialog::updateGroupCheckState(const PrivacyGroup &group)
{
    int checkedCount = 0;
    for (const auto field : group.fields) {
        checkedCount += field->isChecked() ? 1 : 0;
    }

    const Qt::CheckState state =
        checkedCount == 0 ? Qt::Unchecked
        : checkedCount == group.fields.size() ? Qt::Checked
                                              : Qt::PartiallyChecked;
    group.groupCheckBox->blockSignals(true);
    group.groupCheckBox->setCheckState(state);
    group.groupCheckBox->blockSignals(false);
}

void DiagnosticSessionDialog::updateStartButton()
{
    const bool hasDescription = !m_problemDescriptionEdit->toPlainText().trimmed().isEmpty();
    const bool hasOutputDirectory = !m_outputDirectoryEdit->text().trimmed().isEmpty();
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(hasDescription && hasOutputDirectory);

    if (!hasDescription) {
        m_validationLabel->setText("Enter a problem description to continue.");
    } else if (!hasOutputDirectory) {
        m_validationLabel->setText("Choose an output folder to continue.");
    } else {
        m_validationLabel->clear();
    }
}

void DiagnosticSessionDialog::chooseOutputDirectory()
{
    const QString currentPath = m_outputDirectoryEdit->text().trimmed();
    const QFileInfo currentInfo(currentPath);
    const QString initialPath = currentInfo.absolutePath().isEmpty()
            ? QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
            : currentInfo.absolutePath();

    const QString parentDirectory = QFileDialog::getExistingDirectory(
        this, "Choose where to save diagnostic files", initialPath);
    if (parentDirectory.isEmpty()) {
        return;
    }

    QString folderName = currentInfo.fileName();
    if (folderName.isEmpty()) {
        const QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss");
        folderName = QString("pico-asha-diagnostic-%1").arg(timestamp);
    }
    m_outputDirectoryEdit->setText(QDir(parentDirectory).filePath(folderName));
}
