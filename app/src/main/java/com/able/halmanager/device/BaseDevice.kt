package com.able.halmanager.device

/**
 * Create by 赵思琦 on 2022/5/23
 * email zsqandzyr@gmail.com
 */
abstract class BaseDevice<T> {

    abstract val deviceModeable: DeviceModeable<T> //获取数据模式
    abstract val serialPortConnection: SerialPortConnection //串口连接
    abstract val data: T //获取的数据

}