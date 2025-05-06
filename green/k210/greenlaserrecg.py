import sensor, image, time, math, lcd
from machine import UART
from fpioa_manager import fm
from modules import ybkey

# 硬件初始化
fm.register(8, fm.fpioa.UART2_TX, force=True)
fm.register(6, fm.fpioa.UART2_RX, force=True)
uart = UART(UART.UART2, 9600, 8, 0, 1, timeout=500, read_buf_len=8192)  # 增大缓冲区[7](@ref)

# 激光点阈值设置
green_laser_thresholds = (60, 100, -128, -10, -128, 127)  # L_min, L_max, A_min, A_max, B_min, B_max

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_hmirror(False)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)

lcd.init()

KEY = ybkey()
points=[]

#flag:0x00表示发送当前红斑位置,0x01表示发送的是记录的角的位置
def pack_data(data, flag):
    header = 0xFF
    footer = 0xFE
    buffer = bytearray([header, flag])
    for i in data:
        buffer.append((i >> 8) & 0xFF) #高字节
        buffer.append(i & 0xFF)        #低字节
    buffer.append(footer)
    return buffer

while(True):
    img = sensor.snapshot()

    # 寻找激光点
    blobs = img.find_blobs([green_laser_thresholds],
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
            data_packet = pack_data([largest_blob.cx(), largest_blob.cy()], 0x00)
            uart.write(data_packet)
            print("Sent:", data_packet)  # 调试输出
            print(
                largest_blob.cx(),
                largest_blob.cy()
            )
        except Exception as e:
            print("UART Error:", e)

        if (KEY.is_press() and len(points) < 4):
            try:
                points.append((largest_blob.cx(), largest_blob.cy()))
                data_packet = pack_data([largest_blob.cx(), largest_blob.cy()], 0x01)
                uart.write(data_packet)
                print("Sent:", data_packet)  # 调试输出
                time.sleep_ms(100)
            except Exception as e:
                print("UART Error:", e)

        print(points)

    lcd.display(img)
