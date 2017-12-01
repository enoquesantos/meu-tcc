#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QObject>

class QFileDialog;

/**
 * @brief The FileDialog class
 * @extends QObject
 * This class show a system dialog to user select a single file and emit a signal
 * to application with the selected file path after user choose the file.
 * The file type can be filtered in the open method.
 */
class FileDialog : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief DesktopGallery
     * The object constructor
     * @param parent *QObject a pointer to the object parent
     */
    explicit FileDialog(QObject *parent = nullptr);

    /**
     * @brief open
     * Open a window to file selection
     * After user choose the file, the selected file will be copied to app directory and
     * the new path will be sent on accepted signal.
     * To prevent the copy from selected file, pass copyToAppDirectory to false.
     * @param copyToAppDirectory bool a flag to decide if the selected file will be copied to the application directory location
     * @param filterTypes QStringList the list of file types to be filter in dialog
     */
    Q_INVOKABLE void open(bool copyToAppDirectory = true, const QStringList &filters = {".png", ".jpg"});

    /**
     * @brief copyFileToApplicationDir
     * Copy the selected file by user to application writable directory (if 'copyToAppDirectory' is set to true in open method)
     * and sent the new path (the copied file or original file path) to application in the accepted signal.
     * @param filePath QString
     */
    Q_INVOKABLE void copyFileToApplicationDir(const QString &filePath);

private slots:
    /**
     * @brief onFileSelected
     * This slot make a connection with QFileDialog::fileSelected signal in the constructor, and
     * delegate the execution to 'copyFileToApplicationDir()' to handle the copy file and notify the 'accepted' signal.
     * @param file QString
     */
    void onFileSelected(const QString &file);

signals:
    /**
     * @brief accepted
     * Emits a signal with absolute selected file path.
     * The path can be used as source in QML Image object (if file is a image).
     * @param fileUrl QString
     */
    void accepted(const QString &fileUrl);

protected:
    /**
     * @brief m_qFileDialog
     * Handle the file selection dialog.
     * In android, this pointer is not needed and is deleted in the constructor.
     */
    QFileDialog *m_qFileDialog;

    /**
     * @brief m_copyToAppDirectory
     * A flag to check if copy or not the selected file to application location.
     */
    bool m_copyToAppDirectory;
};

#endif // FILEDIALOG_H
