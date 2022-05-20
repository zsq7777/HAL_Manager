/*
 * Copyright 2009-2011 Cedric Priscal
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <jni.h>

#include "SerialPort.h"

#include "android/log.h"

static const char *TAG = "serial_port";
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)

//只在当前文件被链接，获取终端波特率
//返回预编译阶段，宏定义的常量
static speed_t getBaudrate(jint baudrate) {
    switch (baudrate) {
        case 0:
            return B0;
        case 50:
            return B50;
        case 75:
            return B75;
        case 110:
            return B110;
        case 134:
            return B134;
        case 150:
            return B150;
        case 200:
            return B200;
        case 300:
            return B300;
        case 600:
            return B600;
        case 1200:
            return B1200;
        case 1800:
            return B1800;
        case 2400:
            return B2400;
        case 4800:
            return B4800;
        case 9600:
            return B9600;
        case 19200:
            return B19200;
        case 38400:
            return B38400;
        case 57600:
            return B57600;
        case 115200:
            return B115200;
        case 230400:
            return B230400;
        case 460800:
            return B460800;
        case 500000:
            return B500000;
        case 576000:
            return B576000;
        case 921600:
            return B921600;
        case 1000000:
            return B1000000;
        case 1152000:
            return B1152000;
        case 1500000:
            return B1500000;
        case 2000000:
            return B2000000;
        case 2500000:
            return B2500000;
        case 3000000:
            return B3000000;
        case 3500000:
            return B3500000;
        case 4000000:
            return B4000000;
        default:
            return -1;
    }
}


JNIEXPORT jobject JNICALL Java_com_able_hallibrary_serialport_SerialPort_open
        (JNIEnv *env, jclass thiz, jstring path, jint baudrate, jint stopBits, jint dataBits,
         jint parity, jint flowCon, jint flags) {

    int fd;

    speed_t speed;

    jobject mFileDescriptor;
    {
        //获取波特率
        speed = getBaudrate(baudrate);
        if (speed == -1) {
            /* TODO: throw an exception */
            LOGE("native方法：Invalid baudrate");
            return NULL;
        }
    }


    {
        jboolean iscopy;
        const char *path_utf = (*env)->GetStringUTFChars(env, path, &iscopy);
        LOGD("native方法：Opening serial port %s with flags 0x%x", path_utf, O_RDWR | flags);
        //打开文件的方式打开串口，成功返回文件描述符，否则返回-1
        //flags O_RDWR | O_NOCTTY | O_NONBLOCK/O_NDELAY   O_NONBLOCK:数据读不到回传-1
        //O_RDWR默认阻塞模式, 增加|O_NONBLOCK为非阻塞模式，差别在于读操作时，没有读到数据返回-1
        fd = open(path_utf, O_RDWR | O_NONBLOCK);
        LOGD("native方法：open() fd = %d", fd);
        (*env)->ReleaseStringUTFChars(env, path, path_utf);
        if (fd == -1) {
            /* Throw an exception */
            LOGE("native方法：Cannot open port");
            /* TODO: throw an exception */
            return NULL;
        }
    }

    /* Configure device */
    {
        struct termios cfg;
        LOGD("native方法：Configuring serial port");
        /*保存测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息*/
        if (tcgetattr(fd, &cfg)) {
            LOGE("native方法：tcgetattr() failed");
            close(fd);
            /* TODO: throw an exception */
            return NULL;
        }
        //设置终端属性
        cfmakeraw(&cfg);
        //设置输入波特率
        cfsetispeed(&cfg, speed);
        //设置输出波特率
        cfsetospeed(&cfg, speed);

        //设置控制模式->
        cfg.c_cflag &= ~CSIZE;
        switch (dataBits) {
            case 5:
                cfg.c_cflag |= CS5;    //使用5位数据位
                break;
            case 6:
                cfg.c_cflag |= CS6;    //使用6位数据位
                break;
            case 7:
                cfg.c_cflag |= CS7;    //使用7位数据位
                break;
            case 8:
                cfg.c_cflag |= CS8;    //使用8位数据位
                break;
            default:
                cfg.c_cflag |= CS8;
                break;
        }

        switch (parity) {
            case 0:
                cfg.c_cflag &= ~PARENB;    //无奇偶校验
                break;
            case 1:
                cfg.c_cflag |= (PARODD | PARENB);   //奇校验
                break;
            case 2:
                //设置输入模式
                cfg.c_iflag &= ~(IGNPAR | PARMRK); // 偶校验
                cfg.c_iflag |= INPCK;
                //控制模式
                cfg.c_cflag |= PARENB;
                cfg.c_cflag &= ~PARODD;
                break;
            default:
                cfg.c_cflag &= ~PARENB;
                break;
        }

        switch (stopBits) {
            case 1:
                cfg.c_cflag &= ~CSTOPB;    //1位停止位
                break;
            case 2:
                cfg.c_cflag |= CSTOPB;    //2位停止位
                break;
            default:
                break;
        }

        // hardware flow control
        switch (flowCon) {
            case 0:
                cfg.c_cflag &= ~CRTSCTS;    //不使用流控
                break;
            case 1:
                cfg.c_cflag |= CRTSCTS;    //硬件流控
                break;
            case 2:
                cfg.c_cflag |= IXON | IXOFF | IXANY;    //软件流控
                break;
            default:
                cfg.c_cflag &= ~CRTSCTS;
                break;
        }

        //激活新配置
        if (tcsetattr(fd, TCSANOW, &cfg)) {
            LOGE("native方法：tcsetattr() failed");
            close(fd);
            /* TODO: throw an exception */
            return NULL;
        }
    }

    /* Create a corresponding file descriptor */
    {
        jclass cFileDescriptor = (*env)->FindClass(env, "java/io/FileDescriptor");
        jmethodID iFileDescriptor = (*env)->GetMethodID(env, cFileDescriptor, "<init>", "()V");
        jfieldID descriptorID = (*env)->GetFieldID(env, cFileDescriptor, "descriptor", "I");
        mFileDescriptor = (*env)->NewObject(env, cFileDescriptor, iFileDescriptor);
        (*env)->SetIntField(env, mFileDescriptor, descriptorID, (jint) fd);
    }

    return mFileDescriptor;
}




/*
 * Class:     cedric_serial_SerialPort
 * Method:    close
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_able_hallibrary_serialport_SerialPort_close
        (JNIEnv *env, jobject thiz) {
    jclass SerialPortClass = (*env)->GetObjectClass(env, thiz);
    jclass FileDescriptorClass = (*env)->FindClass(env, "java/io/FileDescriptor");

    jfieldID mFdID = (*env)->GetFieldID(env, SerialPortClass, "mFd", "Ljava/io/FileDescriptor;");
    jfieldID descriptorID = (*env)->GetFieldID(env, FileDescriptorClass, "descriptor", "I");

    jobject mFd = (*env)->GetObjectField(env, thiz, mFdID);
    jint descriptor = (*env)->GetIntField(env, mFd, descriptorID);

    LOGD("native方法：close(fd = %d)", descriptor);
    close(descriptor);
}