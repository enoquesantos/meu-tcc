#ifndef JAVA_CPP_CONNECT_H
#define JAVA_CPP_CONNECT_H

#include <jni.h>
#include <QAndroidJniEnvironment>
#include "../core/app.h"

/**
 * @brief eventNotify
 * @param env JNIEnv* set the JNI Environment pointer.
 * @param obj jobject a instance of java Object
 * @param eventName jstring the event name fired by java native method
 * @param eventData jstring the event argument data fired by java native method as string
 */
static void eventNotify(JNIEnv *env, jobject obj, jstring eventName, jstring eventData)
{
    Q_UNUSED(env)
    Q_UNUSED(obj)
    App::fireEventNotify(env->GetStringUTFChars(eventName,0), env->GetStringUTFChars(eventData,0));
}

/**
 * JNINativeMethod methodsArray
 * Keep a list of java native methods to be connected in JNI_OnLoad.
 * The first argument is the java native method name as string.
 * The second argument is the method parameter and the return type.
 * The third argument is the c++ function to be invokable when the java native method is called.
 */
static JNINativeMethod methodsArray[] = {
    {"eventNotify", "(Ljava/lang/String;Ljava/lang/String;)V", (void *) eventNotify}
};

/**
 * The VM calls JNI_OnLoad when the native library is loaded (for example, through System.loadLibrary(QtApplication.so)).
 * Check: https://stackoverflow.com/questions/13509961/which-method-eventually-calls-jni-onload
 */
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    (void)(reserved);
    JNIEnv* env;
    jclass javaClass;

    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
        return JNI_ERR;

    javaClass = env->FindClass("org/qtproject/qt5/android/bindings/ActivityToApplication");
    if (!javaClass)
        return JNI_ERR;

    if (env->RegisterNatives(javaClass, methodsArray, sizeof(methodsArray) / sizeof(methodsArray[0])) < 0)
        return JNI_ERR;

    return JNI_VERSION_1_6;
}

#endif // JAVA_CPP_CONNECT_H
