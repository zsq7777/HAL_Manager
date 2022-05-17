#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_able_halmanager_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "C++";
    return env->NewStringUTF(hello.c_str());
}