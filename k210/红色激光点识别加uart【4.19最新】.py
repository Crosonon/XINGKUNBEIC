import sensor, image, time, math, lcd
from machine import UART
from fpioa_manager import fm

# 硬件初始化
fm.register(8, fm.fpioa.UART2_TX, force=True)
fm.register(6, fm.fpioa.UART2_RX, force=True)
uart = UART(UART.UART2, 9600, 8, 0, 1, timeout=500, read_buf_len=8192)  # 增大缓冲区[7](@ref)

# 激光点阈值设置
red_laser_thresholds = (60, 100, 40, 127, -128, 127)

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_hmirror(True)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)

lcd.init()

def pack_data(x, y):
    header = 0xFF
    footer = 0xFE
    return bytearray([
        header,
        (x >> 8) & 0xFF,  # X高字节
        x & 0xFF,          # X低字节
        (y >> 8) & 0xFF,  # Y高字节
        y & 0xFF,          # Y低字节
        footer
    ])

while(True):
    img = sensor.snapshot()

    # 寻找激光点
    blobs = img.find_blobs([red_laser_thresholds],
                          pixels_threshold=20,
                          area_threshold=20,
                          merge=True)

    if blobs:
        largest_blob = max(blobs, key=lambda b: b.pixels())

        # 绘制图形界面
        img.draw_rectangle(largest_blob.rect(), color=(0,255,0), thickness=2)
        img.draw_cross(largest_blob.cx(), largest_blob.cy(), color=(0,255,0), size=10)
        coord_str = "X:%03d Y:%03d" % (largest_blob.cx(), largest_blob.cy())
        img.draw_string(largest_blob.cx()+2, largest_blob.cy()-20, coord_str,
                        color=(255,255,255), scale=2, mono_space=False)

        # 串口发送数据（每次检测到都发送）
        try:
            data_packet = pack_data(largest_blob.cx(), largest_blob.cy())
            uart.write(data_packet)
            print("Sent:", data_packet)  # 调试输出
            print(
                largest_blob.cx(),
                largest_blob.cy()
            )
        except Exception as e:
            print("UART Error:", e)

    lcd.display(img)
