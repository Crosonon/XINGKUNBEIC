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

# 颜色阈值
RED_LASER_THRESHOLD = (60, 100, 40, 127, -128, 127)
GREEN_LASER_THRESHOLD = (30, 100, -64, -8, -32, 32)

# 协议解析状态机变量
rx_state = 0
rx_data_len = 0
rx_buffer = []
rx_checksum = 0
trigger_rect = 0
rect_coords = []

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
last_laser_send = time.ticks_ms()  # 统一发送时间戳
send_red_next = True               # 交替发送标志
laser_interval = 10

while True:
    clock.tick()
    img = sensor.snapshot()
    current_time = time.ticks_ms()

    # UART协议解析状态机
    while uart.any():
        byte = uart.read(10)
        if byte:
            if byte[0] == 0xff and byte[-1] == 0xfe:
                if trigger_rect == 0:
                    trigger_rect = 1
                if trigger_rect == 2 and len(byte) >= 3 and 0 <= int(byte[1]) <= 3:
                    uart.write(pack_data(rect_coords[int(byte[1])], 0x01))


    # 矩形检测及发送
    if trigger_rect == 1:
        rects = img.find_rects(threshold=3500, roi=(40,30,200,150))
        if rects:
            for r in rects:
                if r.w() > 30 and r.h() > 30:
                    corners = r.corners()
                    points = list(corners)

                    cx = sum(p[0] for p in points) / 4
                    cy = sum(p[1] for p in points) / 4

                    def get_angle(p):
                        dx = p[0] - cx
                        dy = cy - p[1]
                        return math.atan2(dy, dx)

                    points = sorted(points, key=lambda p: get_angle(p), reverse=True)

                    for idx, (x, y) in enumerate(points):
                        rect_coords.append((x, y))
                        img.draw_circle(x, y, 3, color=(0,0,255))
                        label = "X{}".format(idx+1)
                        text_x = x + 5 if x < 280 else x - 30
                        text_y = y + 5 if y < 220 else y - 15
                        img.draw_string(text_x, text_y, label,
                                      color=(255,255,255),
                                      scale=1.2,
                                      mono_space=False,
                                      bg_color=(0,0,255))
                    img.draw_rectangle(r.rect(), color=(255,0,0), thickness=2)
                    trigger_rect = 2
                    break

    # 激光检测逻辑
    if trigger_rect == 0 or trigger_rect == 2:
        red_coords = None
        green_coords = None

        # 红色激光检测
        red_blobs = img.find_blobs([RED_LASER_THRESHOLD],
                                pixels_threshold=15,
                                area_threshold=15,
                                merge=True,
                                margin=5)
        if red_blobs:
            max_red = max(red_blobs, key=lambda b: (b.area(), -b.y()))
            red_coords = (max_red.cx(), max_red.cy())
            img.draw_circle(red_coords[0], red_coords[1], 8, color=(255,0,0))
            img.draw_string(red_coords[0]+10, red_coords[1]+10,
                          "R:({},{})".format(*red_coords),
                          color=(255,0,0), scale=1.5, bg_color=(0,0,0))

        # 绿色激光检测
        green_blobs = img.find_blobs([GREEN_LASER_THRESHOLD],
                                   pixels_threshold=15,
                                   area_threshold=15,
                                   merge=True,
                                   margin=5)
        if green_blobs:
            max_green = max(green_blobs, key=lambda b: (b.area(), -b.y()))
            green_coords = (max_green.cx(), max_green.cy())
            img.draw_circle(green_coords[0], green_coords[1], 8, color=(0,255,0))
            img.draw_string(green_coords[0]+10, green_coords[1]+10,
                          "G:({},{})".format(*green_coords),
                          color=(0,255,0), scale=1.5, bg_color=(0,0,0))

        # 交替发送逻辑（核心修改部分）
        if current_time - last_laser_send >= 200:
            if send_red_next:
                if red_coords:
                    uart.write(pack_data(red_coords, 0x02))
                    print("Red sent:", red_coords)
                    send_red_next = False
                    last_laser_send = current_time
                elif green_coords:  # 红色不存在时尝试发送绿色
                    uart.write(pack_data(green_coords, 0x03))
                    print("Green sent:", green_coords)
                    send_red_next = True
                    last_laser_send = current_time
            else:
                if green_coords:
                    uart.write(pack_data(green_coords, 0x03))
                    print("Green sent:", green_coords)
                    send_red_next = True
                    last_laser_send = current_time
                elif red_coords:  # 绿色不存在时尝试发送红色
                    uart.write(pack_data(red_coords, 0x02))
                    print("Red sent:", red_coords)
                    send_red_next = False
                    last_laser_send = current_time

    # 按键处理
    if KEY.is_press():
        trigger_rect = 1 if trigger_rect == 0 else 0

    lcd.display(img)
    lcd.draw_string(0, 0, "YIBAI4", lcd.RED, lcd.WHITE)
    lcd.draw_string(220, 0, "TR:{}".format(trigger_rect), lcd.RED, lcd.WHITE)
