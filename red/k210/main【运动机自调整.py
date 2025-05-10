import sensor
import image
import time
import lcd, math
from machine import UART
from fpioa_manager import fm
from modules import ybkey

# 硬件初始化
fm.register(8, fm.fpioa.UART2_TX, force=True)
fm.register(6, fm.fpioa.UART2_RX, force=True)

uart = UART(UART.UART2, 9600, 8, 0, 1, timeout=500, read_buf_len=8192)

# 显示初始化
lcd.init(freq=15000000)
lcd.rotation(0)
lcd.clear()

# 按键初始化
KEY = ybkey()

# 摄像头初始化
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_vflip(False)
sensor.set_hmirror(False)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
sensor.set_auto_exposure(False, exposure=20000)
sensor.skip_frames(time=2000)

# 优化后的红色激光LAB阈值
RED_LASER_THRESHOLD = (60, 100, 40, 127, -128, 127)

# 协议解析状态机变量
rx_state = 0
rx_data_len = 0
rx_buffer = []
rx_checksum = 0
trigger_rect = 1
rect_coords = []
new_rect_coords = []

def pack_data(data, flag):
    """数据打包函数"""
    header = 0xFF
    footer = 0xFE
    buffer = bytearray([header, flag])
    for num in data:
        num = max(0, min(num, 320))
        buffer.append((num >> 8) & 0xFF)
        buffer.append(num & 0xFF)
    buffer.append(footer)
    buffer.append(footer)
    return buffer

# 时间控制参数
clock = time.clock()
last_send = time.ticks_ms()
last_laser_time = time.ticks_ms()
laser_interval = 10

while True:
    clock.tick()
    img = sensor.snapshot()
    current_time = time.ticks_ms()
    laser_coords = None

    # UART协议解析状态机
    while uart.any():
        byte = uart.read(10)
        if byte:
            if byte[0] == 0xff and byte[-1] == 0xfe:
                if trigger_rect == 0:
                    trigger_rect = 1
                    print('set Trigger_rect 1')
                if trigger_rect == 2 and len(byte) >= 3 and 0 <= int(byte[1]) <= 3:
                    uart.write(pack_data(new_rect_coords[int(byte[1])], 0x01))

    # 矩形检测及发送
    if trigger_rect == 1:
        img.negate()
        gray_img = img.to_grayscale(copy=True)
        gray_img.negate()
        rects = gray_img.find_rects(threshold=6800, roi=(70,40,220,160))

        if rects:
            for r in rects:
                if r.w() > 40 and r.h() > 40:
                    corners = r.corners()
                    points = list(corners)
                    cx = sum(p[0] for p in points) / 4
                    cy = sum(p[1] for p in points) / 4

                    def get_angle(p):
                        dx = p[0] - cx
                        dy = cy - p[1]
                        return math.atan2(dy, dx)

                    points = sorted(points, key=lambda p: get_angle(p), reverse=True)

                    # 计算新坐标
                    rect_coords = []
                    new_rect_coords = []
                    for (x, y) in points:
                        dx = cx - x
                        dy = cy - y
                        distance = math.sqrt(dx**2 + dy**2)
                        if distance > 0:
                            new_x = x + (dx / distance) * 2
                            new_y = y + (dy / distance) * 2
                        else:
                            new_x = x
                            new_y = y
                        new_x = int(round(new_x))
                        new_y = int(round(new_y))
                        rect_coords.append((x, y))
                        new_rect_coords.append((new_x, new_y))

                    # 绘制原始顶点
                    for idx, (x, y) in enumerate(rect_coords):
                        img.draw_circle(x, y, 3, color=(0,0,255))
                        label = "X{}".format(idx+1)
                        text_x = x + 5 if x < 280 else x - 30
                        text_y = y + 5 if y < 220 else y - 15
                        img.draw_string(text_x, text_y, label,
                                      color=(255,255,255),
                                      scale=1.2,
                                      mono_space=False,
                                      bg_color=(0,0,255))

                    # 绘制原始矩形
                    img.draw_rectangle(r.rect(), color=(255,0,0), thickness=2)

                    # 绘制新矩形边框（绿色）
                    for i in range(4):
                        start = new_rect_coords[i]
                        end = new_rect_coords[(i+1)%4]
                        img.draw_line(start[0], start[1], end[0], end[1], color=(0,255,0), thickness=2)



                    trigger_rect = 2
                    print("Original:", rect_coords)
                    print("New:", new_rect_coords)
                    break

    if trigger_rect == 0 or trigger_rect == 2:
        # 激光检测逻辑保持不变
        if current_time - last_laser_time > laser_interval:
            blobs = img.find_blobs([RED_LASER_THRESHOLD],
                                pixels_threshold=15,
                                area_threshold=15,
                                merge=True,
                                margin=5)
            if blobs:
                max_blob = max(blobs, key=lambda b: (b.area(), -b.y()))
                laser_coords = (max_blob.cx(), max_blob.cy())
                img.draw_circle(laser_coords[0], laser_coords[1], 8, color=(255,0,0))
                img.draw_string(laser_coords[0]+10, laser_coords[1]+10,
                              "R:({},{})".format(*laser_coords),
                              color=(255,0,0),
                              scale=1.5,
                              bg_color=(0,0,0))
                last_laser_time = current_time

                if current_time - last_send >= 200 and laser_coords:
                    uart.write(pack_data(laser_coords, 0x02))
                    print("Laser Sent:", laser_coords)
                    last_send = current_time

    lcd.display(img)
    lcd.draw_string(0, 0, "YIBAI4", lcd.RED, lcd.WHITE)
    lcd.draw_string(220, 0, "TR:{}".format(trigger_rect), lcd.RED, lcd.WHITE)
