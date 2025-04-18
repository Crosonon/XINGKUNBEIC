import sensor, image, time, math, lcd

# 更改阈值
#thresholds = [(7, 31, -128, 7, -128, 15)]
red_laser_thresholds = (60, 100,  40, 127,  -128, 127)  # 高亮度红色区域

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)  # 320x240
#sensor.set_vflip(True)            # 根据摄像头安装方向调整
sensor.set_hmirror(True)
sensor.skip_frames(time=2000)     # 等待感光元件稳定
sensor.set_auto_gain(False)       # 关闭自动增益
sensor.set_auto_whitebal(False)   # 关闭白平衡

lcd.init()

while(True):
    img = sensor.snapshot()

    # 寻找激光点（提高灵敏度）
    blobs = img.find_blobs([red_laser_thresholds],
                           pixels_threshold=20,   # 最小像素数
                           area_threshold=20,     # 最小区域
                           merge=True)             # 合并相邻斑点

    if blobs:
        # 取最大的斑点（避免多个误检）
        largest_blob = max(blobs, key=lambda b: b.pixels())

        # 绘制小框（比斑点稍大2像素）
        img.draw_rectangle(largest_blob.rect(), color=(0,255,0), thickness=2)

        # 绘制中心十字
        img.draw_cross(largest_blob.cx(), largest_blob.cy(), color=(0,255,0), size=10)

        # 打印坐标信息（带时间戳）
        print("[{:.1f}] Laser @ ({:3d}, {:3d})".format(
            time.ticks_ms()/1000,
            largest_blob.cx(),
            largest_blob.cy()
        ))

    lcd.display(img)
