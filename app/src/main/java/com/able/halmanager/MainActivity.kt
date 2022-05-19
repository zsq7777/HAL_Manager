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

        binding.btnOpen.setOnClickListener {
            val serialPort = SerialPort(File("/dev/ttyS2"), 115200, 0)
            val mInputStream = serialPort.inputStream
            val outputStream = serialPort.outputStream
            val thread = Thread(Runnable {
                while (true) {
                    if (mInputStream == null) return@Runnable
                    var buffer: ByteArray
                    val im: Int = mInputStream.available()
                    if (im > 0) {
                        buffer = ByteArray(im)
                        val size: Int = mInputStream.read(buffer)
                        val str = String(buffer)
                        Log.i("读取到的String", str)
                    }
                    try {
                        Thread.sleep(1000)
                    } catch (e: InterruptedException) {
                        e.printStackTrace()
                    }
                }
            }).start()
        }
    }

}