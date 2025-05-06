import sensor
import image
import time
import lcd, math
from machine import UART
from fpioa_manager import fm
from modules import ybkey

# 硬件初始化
fm.register(6, fm.fpioa.UART2_TX)
fm.register(8, fm.fpioa.UART2_RX)

# 显示初始化
lcd.init(freq=15000000)
lcd.rotation(0)
lcd.clear()

# 摄像头初始化
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_vflip(False)
sensor.set_hmirror(False)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)

# 按键初始化
KEY = ybkey()
points = []

# 串口初始化
uart = UART(UART.UART2, 115200, 8, 0, 0, timeout=1000)

# 颜色阈值
GREEN_LASER_THRESHOLD = (30, 100, -64, -8, -32, 32)

def pack_data(data, flag):
    """数据打包函数"""
    header = 0xFF
    footer = 0xFE
    buffer = bytearray([header, flag])
    for num in data:
        num = max(0, min(num, 255))
        buffer.append((num >> 8) & 0xFF)
        buffer.append(num & 0xFF)
    buffer.append(footer)
    return buffer

# 时间控制参数
clock = time.clock()
last_send = time.ticks_ms()
last_rect_time = time.ticks_ms()
last_laser_time = time.ticks_ms()
rect_interval = 30
laser_interval = 10

while True:
    clock.tick()
    img = sensor.snapshot()
    current_time = time.ticks_ms()
    rect_coords = None
    laser_coords = None

    # 矩形检测
    if current_time - last_rect_time > rect_interval:
        rects = img.find_rects(threshold=5000, roi=(40,30,240,180))
        if rects:
            for r in rects:
                if r.w() > 40 and r.h() > 40:
                    corners = r.corners()
                    points = list(corners)

                    # 顺时针排序
                    cx = sum(p[0] for p in points) / 4
                    cy = sum(p[1] for p in points) / 4

                    def get_angle(p):
                        dx = p[0] - cx
                        dy = cy - p[1]
                        return math.atan2(dy, dx)

                    points = sorted(points, key=lambda p: get_angle(p), reverse=True)

                    rect_coords = []
                    # ========== 新增顶点标签绘制 ==========
                    for idx, (x, y) in enumerate(points):
                        rect_coords.append(x)
                        rect_coords.append(y)
                        img.draw_circle(x, y, 3, color=(0,0,255))

                        # 生成标签文本 X1-X4
                        label = "X{}".format(idx+1)
                        # 动态调整标签位置
                        text_x = x + 5 if x < 280 else x - 30  # 防止右侧溢出
                        text_y = y + 5 if y < 220 else y - 15  # 防止底部溢出
                        # 绘制带背景的标签
                        img.draw_string(text_x, text_y, label,
                                      color=(255,255,255),  # 白色文字
                                      scale=1.2,            # 放大字号
                                      mono_space=False,
                                      bg_color=(0,0,255))   # 蓝色背景
                    # ====================================

                    img.draw_rectangle(r.rect(), color=(255,0,0), thickness=2)
                    last_rect_time = current_time
                    break

    # 激光检测
    if current_time - last_laser_time > laser_interval:
        blobs = img.find_blobs([GREEN_LASER_THRESHOLD],
                            pixels_threshold=30,
                            area_threshold=30,
                            merge=True,
                            margin=10)
        if blobs:
            max_blob = max(blobs, key=lambda b: b.area())
            laser_coords = [max_blob.cx(), max_blob.cy()]
            img.draw_circle(laser_coords[0], laser_coords[1], 5, color=(0,255,0))
            last_laser_time = current_time

            img.draw_string(laser_coords[0]+5, laser_coords[1]+5,
                          "L:({},{})".format(*laser_coords),
                          color=(0,255,0),
                          scale=1)

    # 数据发送
    if current_time - last_send >= 200:
        if rect_coords is not None:
            uart.write(pack_data(rect_coords, 0x01))
            print("Rect Sent:", rect_coords)
        if laser_coords is not None:
            uart.write(pack_data(laser_coords, 0x02))
            print("Laser Sent:", laser_coords)
        last_send = current_time

    lcd.display(img)
