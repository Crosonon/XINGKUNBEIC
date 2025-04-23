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
pointmanual = []

# 串口初始化
uart = UART(UART.UART2, 115200, 8, 0, 0, timeout=1000)

# 颜色阈值
GREEN_LASER_THRESHOLD = (30, 100, -64, -8, -32, 32)

# 协议解析状态机变量[new
rx_state = 0         # 接收状态（0-等待包头）
rx_data_len = 0      # 数据长度
rx_buffer = []       # 数据缓冲区
rx_checksum = 0      # 校验和计算
trigger_rect = False # 矩形检测触发标志，完成一次检测后自动清零

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
last_laser_time = time.ticks_ms()
laser_interval = 10

while True:
    clock.tick()
    img = sensor.snapshot()
    current_time = time.ticks_ms()
    laser_coords = None

    # UART协议解析状态机【new,待进一步修改
    while uart.any():
        byte = uart.read(1)[0]

        if rx_state == 0:          # 等待包头
            if byte == 0xAA:
                rx_state = 1
                rx_buffer = []
                rx_checksum = 0

        elif rx_state == 1:        # 获取数据长度
            rx_data_len = byte
            rx_state = 2 if rx_data_len > 0 else 3

        elif rx_state == 2:        # 接收数据内容
            rx_buffer.append(byte)
            rx_checksum += byte
            if len(rx_buffer) >= rx_data_len:
                rx_state = 3

        elif rx_state == 3:        # 验证校验和
            if rx_checksum == byte:
                rx_state = 4
            else:
                rx_state = 0  # 校验失败

        elif rx_state == 4:        # 验证包尾
            if byte == 0x55:
                trigger_rect = True  # 触发矩形检测
            rx_state = 0

    # 矩形检测及发送（仅在收到指令时执行）
    if trigger_rect:
        rect_coords = None
        rects = img.find_rects(threshold=5000, roi=(40,30,240,180))
        if rects:
            for r in rects:
                if r.w() > 40 and r.h() > 40:
                    corners = r.corners()
                    points = list(corners)

                    # 坐标排序逻辑
                    cx = sum(p[0] for p in points) / 4
                    cy = sum(p[1] for p in points) / 4

                    def get_angle(p):
                        dx = p[0] - cx
                        dy = cy - p[1]
                        return math.atan2(dy, dx)

                    points = sorted(points, key=lambda p: get_angle(p), reverse=True)

                    rect_coords = []
                    for idx, (x, y) in enumerate(points):
                        rect_coords.append(x)
                        rect_coords.append(y)
                        img.draw_circle(x, y, 3, color=(0,0,255))
                        label = "X{}".format(idx+1)
                        text_x = x + 5 if x < 280 else x - 30
                        text_y = y + 5 if y < 220 else y - 15
                        img.draw_string(text_x, text_y, label,
                                      color=(255,255,255),
                                      scale=1.2,
                                      bg_color=(0,0,255))

                    img.draw_rectangle(r.rect(), color=(255,0,0), thickness=2)
                    break

        # 发送矩形坐标
        if rect_coords:
            uart.write(pack_data(rect_coords, 0x01))
            print("Rect Sent:", rect_coords)
        trigger_rect = False  # 重置触发标志

    # 持续激光检测-----------------------------------------
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
            img.draw_string(laser_coords[0]+5, laser_coords[1]+5,
                          "L:({},{})".format(*laser_coords),
                          color=(0,255,0),
                          scale=1)
            last_laser_time = current_time

    # 定时发送激光坐标
    if current_time - last_send >= 200:
        if laser_coords:
            uart.write(pack_data(laser_coords, 0x02))
            print("Laser Sent:", laser_coords)
        last_send = current_time

    # 按键检测（仅在按下时执行）----------------------
    if KEY.is_press():
    # 状态判断：达到4个点时清空数组
         if len(pointmanual) >= 4:
            pointmanual.clear()
            print("Array cleared, ready for new points")
            #串行终端print测试用，后续可删pointmanual数组相关内容

    # 坐标采集与发送（支持清空后立即记录）
         if len(pointmanual) < 4:
           try:
            # 获取当前光点坐标
              current_coords = (max_blob.cx(), max_blob.cy())

            # 存储坐标
              pointmanual.append((max_blob.cx(), max_blob.cy()))
            # 串口发送（仿照激光坐标发送结构）
              uart.write(pack_data(current_coords, 0x03))
              print("Sent:",current_coords)
              time.sleep_ms(100)  # 防抖动处理

           except Exception as e:
              print("UART Error:", e)  # 细化异常类型

    print("pointmanual:",pointmanual)

    lcd.display(img)
