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

uart = UART(UART.UART2, 9600, 8, 0, 1, timeout=500, read_buf_len=8192)  # 增大缓冲区[7](@ref)

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
sensor.set_auto_gain(False)       # 关闭自动增益
sensor.set_auto_whitebal(False)   # 关闭白平衡
sensor.set_auto_exposure(False, exposure=20000)  # 降低曝光使背景更暗
sensor.skip_frames(time=2000)

# 优化后的红色激光LAB阈值
RED_LASER_THRESHOLD = (60, 100, 40, 127, -128, 127)  # 优化后的LAB阈值范围


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
                     # 发送坐标时补充flag参数
                        uart.write(pack_data(rect_coords[int(byte[1])], 0x01))
                        #print('send:', rect_coords[int(byte[1])])


    # 矩形检测及发送
    if trigger_rect == 1:

    # 矩形检测（ROI （识别感兴趣区域）设置为20,15开始，宽225，高165的区域）

         # 转换为灰度图以提高处理速度（可选）
        gray_img = img.to_grayscale(copy=True)

        rects = gray_img.find_rects(threshold=7000 ,roi=(30,30,220,170))

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

                    # ========== 新增顶点标签绘制 ==========
                    for idx, (x, y) in enumerate(points):
                        rect_coords.append((x, y))
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

                    trigger_rect = 2
                    print(rect_coords)
                    print('set Trigger_rect 2')
                    break

    if trigger_rect == 0 or trigger_rect == 2:
        # 优化后的红色激光检测
        if current_time - last_laser_time > laser_interval:
            # 使用LAB颜色空间进行检测
            blobs = img.find_blobs([RED_LASER_THRESHOLD],
                                pixels_threshold=15,
                                area_threshold=15,
                                merge=True,
                                margin=5)

            if blobs:
                # 选择面积最大且位置最靠上的blob
                max_blob = max(blobs, key=lambda b: (b.area(), -b.y()))
                laser_coords = (max_blob.cx(), max_blob.cy())

                # 增强显示效果
                img.draw_circle(laser_coords[0], laser_coords[1], 8, color=(255,0,0))
                img.draw_string(laser_coords[0]+10, laser_coords[1]+10,
                              "R:({},{})".format(*laser_coords),
                              color=(255,0,0),
                              scale=1.5,
                              bg_color=(0,0,0))
                last_laser_time = current_time

                if current_time - last_send >= 200:
                   if laser_coords:
                      uart.write(pack_data(laser_coords, 0x02))
                      print("Laser Sent:", laser_coords)
                      last_send = current_time
    # 定时发送坐标

    # 按键处理
    if KEY.is_press():
        trigger_rect = 1 if trigger_rect == 0 else 0

    lcd.display(img)
    lcd.draw_string(0, 0, "YIBAI4", lcd.RED, lcd.WHITE)
    lcd.draw_string(220, 0, "TR:{}".format(trigger_rect), lcd.RED, lcd.WHITE)  # 新增状态显示
