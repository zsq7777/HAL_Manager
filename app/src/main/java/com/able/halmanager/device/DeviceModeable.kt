package com.able.halmanager.device

/**
 * 设备通信模式 R:请求类型 T：返回类型
 * Create by 赵思琦 on 2022/5/23
 * email zsqandzyr@gmail.com
 */

interface DeviceModeable<T> {
    //获取的数据
    val data: T
}

//应答模式
interface AnswerMode<R, T> : DeviceModeable<T> {
    fun command(r: R)
}

//主动上报模式
interface ReportMode<T> : DeviceModeable<T>