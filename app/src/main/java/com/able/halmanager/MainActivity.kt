package com.able.halmanager

import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.able.hallibrary.serialport.SerialPort
import com.able.halmanager.databinding.ActivityMainBinding
import java.io.File

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        val serialPort = SerialPort(File("/dev/ttyS2"), 115200)
        val mInputStream = serialPort.inputStream
        val mOutputStream = serialPort.outputStream
        Thread(Runnable {
            while (true) {
                if (mInputStream == null) return@Runnable
                var buffer: ByteArray
                val im: Int = mInputStream.available()
                if (im > 0) {
                    buffer = ByteArray(im)
                    val size: Int = mInputStream.read(buffer)
                    val str = String(buffer)
                    Log.i("串口读取", str)
                }
                try {
                    Thread.sleep(1000)
                } catch (e: InterruptedException) {
                    e.printStackTrace()
                }
            }
        }).start()

        binding.btnOpen.setOnClickListener {
            Thread(Runnable {
                while (true) {
                    Log.i("串口写入","");
                    try {
                        val sendData = StringBuilder("{\"requestID\":1}").toString().toByteArray()
                        mOutputStream.write(sendData)
                        Thread.sleep(500)
                    } catch (e: Exception) {
                        e.printStackTrace()
                    }
                }
            }).start()
        }
    }

}