package com.able.halmanager.device

import com.able.hallibrary.serialport.SerialPort
import java.io.File
import java.io.InputStream
import java.io.OutputStream

/**
 * Create by 赵思琦 on 2022/5/23
 * email zsqandzyr@gmail.com
 */
data class SerialPortEntity(
    val SerialPortName: String,//串口名
    val baudrate: Int,//比特率
    val stopBits: Int = 1,//停止位
    val dataBits: Int = 8,//数据位
    val parity: Int = 0,//奇偶校验位
    val flowCon: Int = 0//流控
)

class SerialPortConnection(private val serialPortEntity: SerialPortEntity) {
    private var mSerialPort: SerialPort? = null
    private var mInputStream: InputStream? = null
    private var mOutputStream: OutputStream? = null

    fun open() {
        mSerialPort = SerialPort(File(serialPortEntity.SerialPortName), serialPortEntity.baudrate)
        mInputStream = mSerialPort?.inputStream
        mOutputStream = mSerialPort?.outputStream
    }

    fun command(sendData: ByteArray) {
        mOutputStream?.write(sendData)
    }

    fun close() {
        mOutputStream?.close()
        mInputStream?.close()
        mSerialPort?.close()
    }
}