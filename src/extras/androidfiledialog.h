#ifndef AndroidFileDialog_H
#define AndroidFileDialog_H

#include "../core/filedialog.h"

#include <QtAndroidExtras>

/**
 * @brief The AndroidFileDialog class
 * @extends FileDialog
 * @implements QAndroidActivityResultReceiver
 * A class to show the android file dialog with native look and feel.
 */
class AndroidFileDialog : public FileDialog, public QAndroidActivityResultReceiver
{
    Q_OBJECT
public:
    /**
     * @brief AndroidFileDialog
     * The object constructor
     * @param parent *QObject a pointer to the parent QObject
     */
    explicit AndroidFileDialog(QObject *parent = nullptr);

    /**
     * @brief open
     * @overload
     * override the parent open(bool) method to android device gallery.
     * Android not has a window manager, so, each device and version can have a system dialog.
     * So, to open the native from device dialog, we needs to use a java binds using JNI.
     * This method open the android files gallery to selection embedded into current app activity.
     * @param copyToAppDirectory bool set to false to not copy the selected file by user
     * @param filters QStringList the list of files type to filter in the dialog.
     * In android, you can set filters to "*slash*" to show all mime types.
     * Or you can set mupltiple types like: image/\*|application\/pdf|audio/\*
     */
    Q_INVOKABLE void open(bool copyToAppDirectory = true, const QStringList &filters = {"image/*"});

    /**
     * @brief handleActivityResult
     * @overload
     * Handle the android activiy result from QtActivity (android activity)
     * This method get the intent object in data parameter to get the
     * selected file by user
     * @param receiverRequestCode int the value of operation
     * @param resultCode the result code of activity user selection
     * @param data a intent with user selected item as android Intent
     */
    void handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data);
};

#endif // AndroidFileDialog_H
