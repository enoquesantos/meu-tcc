#include "androidfiledialog.h"

#include <QFileDialog>
#include <QtAndroid>
#include <QAndroidJniEnvironment>
#include <QtAndroidExtras/QAndroidJniObject>

AndroidFileDialog::AndroidFileDialog(QObject *parent): FileDialog(parent)
{
     m_qFileDialog->deleteLater();
}

void AndroidFileDialog::open(bool copyToAppDirectory, const QStringList &filters)
{
    m_copyToAppDirectory = copyToAppDirectory;
    QAndroidJniObject ACTION_PICK = QAndroidJniObject::getStaticObjectField("android/content/Intent", "ACTION_PICK", "Ljava/lang/String;");
    QAndroidJniObject EXTERNAL_CONTENT_URI = QAndroidJniObject::getStaticObjectField("android/provider/MediaStore$Images$Media", "EXTERNAL_CONTENT_URI", "Landroid/net/Uri;");
    QAndroidJniObject intent = QAndroidJniObject("android/content/Intent", "(Ljava/lang/String;Landroid/net/Uri;)V", ACTION_PICK.object<jstring>(), EXTERNAL_CONTENT_URI.object<jobject>());
    if (ACTION_PICK.isValid() && intent.isValid()) {
        intent.callObjectMethod("setType", "(Ljava/lang/String;)Landroid/content/Intent;", QAndroidJniObject::fromString(filters.join(" ")).object<jstring>());
        QtAndroid::startActivity(intent.object<jobject>(), 101, this);
    }
}

void AndroidFileDialog::handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data)
{
    jint RESULT_OK = QAndroidJniObject::getStaticField<jint>("android/app/Activity", "RESULT_OK");
    if (receiverRequestCode == 101 && resultCode == RESULT_OK) {
        QAndroidJniEnvironment env;
        QAndroidJniObject uri = data.callObjectMethod("getData", "()Landroid/net/Uri;");
        QAndroidJniObject mediaStore = QAndroidJniObject::getStaticObjectField("android/provider/MediaStore$MediaColumns", "DATA", "Ljava/lang/String;");

        jobjectArray stringObject = (jobjectArray) env->NewObjectArray(1, env->FindClass("java/lang/String"), NULL);
        env->SetObjectArrayElement(stringObject, 0, env->NewStringUTF(mediaStore.toString().toStdString().c_str()));

        QAndroidJniObject contentResolver = QtAndroid::androidActivity().callObjectMethod("getContentResolver", "()Landroid/content/ContentResolver;");
        QAndroidJniObject cursor = contentResolver.callObjectMethod("query", "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;", uri.object<jobject>(), stringObject, NULL, NULL, NULL);

        jint columnIndex = cursor.callMethod<jint>("getColumnIndex", "(Ljava/lang/String;)I", mediaStore.object<jstring>());
        cursor.callMethod<jboolean>("moveToFirst", "()Z");
        copyFileToApplicationDir(cursor.callObjectMethod("getString", "(I)Ljava/lang/String;", columnIndex).toString());
        cursor.callMethod<void>("close", "()V");
    }
}
