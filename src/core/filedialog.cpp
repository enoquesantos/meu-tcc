#include "filedialog.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>

FileDialog::FileDialog(QObject *parent): QObject(parent)
  ,m_copyToAppDirectory(true)
{
    m_qFileDialog = new QFileDialog;
    m_qFileDialog->setFileMode(QFileDialog::ExistingFiles);
    m_qFileDialog->setDirectory(QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).last());
    connect(m_qFileDialog, SIGNAL(fileSelected(QString)), this, SLOT(onFileSelected(QString)));
}

void FileDialog::onFileSelected(const QString &file)
{
    copyFileToApplicationDir(file);
}

void FileDialog::open(bool copyToAppDirectory, const QStringList &filters)
{
    m_copyToAppDirectory = copyToAppDirectory;
    m_qFileDialog->setNameFilter(filters.join(" "));
    m_qFileDialog->open();
}

void FileDialog::copyFileToApplicationDir(const QString &filePath)
{
    if (!m_copyToAppDirectory) {
        emit accepted(QStringLiteral("file://") + filePath);
        return;
    }

    // get the application writeable directory path
    QString writeablePath(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));

    QDir qdir(writeablePath);
    // if the directory not exists will be create
    if (!qdir.exists())
        qdir.mkpath(writeablePath);

    // get the absolute destination file name
    QString newFilePath(writeablePath.append("/" + QFileInfo(filePath).fileName()));
    newFilePath.remove(QStringLiteral("file://"));

    // QFIle not overwrite the original file if exists and
    // if the image (from newFilePath) exists, will be removed!
    if (QFile::exists(newFilePath))
        QFile::remove(newFilePath);
    bool status = QFile::copy(QString(filePath).remove(QStringLiteral("file://")), newFilePath);

    // notify application with copied file path
    emit accepted(QStringLiteral("file://") + (status ? newFilePath : filePath));
}
