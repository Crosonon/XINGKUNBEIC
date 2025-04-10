import sensor, image, lcd, time, utime
from machine import UART
from fpioa_manager import fm

# 硬件初始化
fm.register(15, fm.fpioa.UART1_TX, force=True)
fm.register(16, fm.fpioa.UART1_RX, force=True)
uart = UART(UART.UART1, 9600, 8, 0, 1, timeout=500, read_buf_len=8192)  # 增大缓冲区[7](@ref)

# 摄像头配置（关键改进）[1,7](@ref)
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
sensor.set_auto_exposure(False, 1400)  # 固定曝光避免闪烁[7](@ref)
sensor.set_contrast(3)            # 提高对比度增强边缘[7](@ref)
sensor.set_brightness(1)          # 适度提亮画面[7](@ref)
sensor.set_saturation(2)          # 提升饱和度增强红色[1](@ref)
#sensor.set_vflip(1)
sensor.set_hmirror(1)
sensor.skip_frames(time=2000)     # 延长稳定化时间[1](@ref)

# LCD初始化
lcd.init()

# 优化后的红色阈值（LAB空间）[1,7](@ref)
RED_THRESHOLD = [
    (68, 100,  35, 127,  -40, 127),  # 主红色区间（增强黄色分量抑制）
    (30, 65,   25, 127,   -5, 50)    # 低亮度红色补偿（如阴影区域）
]

# 性能参数优化
SEND_INTERVAL = 150     # 缩短发送间隔（±30%响应提升）
FRAME_DELAY = 50        # 提升至20fps（需实测稳定性）
MERGING_MARGIN = 20     # 增大色块合并范围[1](@ref)

def find_max_blob(blobs):
    """动态权重评估最大色块"""
    if not blobs: return None
    return max(blobs, key=lambda b: (b.pixels() * 0.7 + b.density() * 0.3))  # 综合像素数和密度[3](@ref)

last_send_time = 0
def should_send():
    global last_send_time
    current = utime.ticks_ms()
    if utime.ticks_diff(current, last_send_time) > SEND_INTERVAL:
        last_send_time = current
        return True
    return False

def draw_info(img, blob):
    """增强可视化效果"""
    if not blob: return
    # 动态绘制参数
    rect_color = (255,0,0) if blob.pixels() > 1500 else (0,255,0)  # 大色块红色框，小色块绿色框
    cross_size = 12 if blob.density() > 0.8 else 8

    img.draw_rectangle(blob.rect(), color=rect_color, thickness=2)
    img.draw_cross(blob.cx(), blob.cy(), color=rect_color, size=cross_size)

    # 坐标显示优化
    coord_str = "X:%03d Y:%03d" % (blob.cx(), blob.cy())
    img.draw_string(blob.x()+2, blob.y()-20, coord_str,
                   color=(255,255,255), scale=2, mono_space=False)

while True:
    img = sensor.snapshot().gamma_corr(1.8)  # Gamma校正增强对比[7](@ref)

    # 多阈值检测优化[1](@ref)
    blobs = img.find_blobs(RED_THRESHOLD,
                          pixels_threshold=500,    # 降低灵敏度
                          area_threshold=500,
                          merge=True,
                          margin=MERGING_MARGIN,
                          x_stride=4,             # 横向扫描步长
                          y_stride=4)

    max_blob = find_max_blob(blobs)

    if max_blob and (max_blob.density() > 0.6):  # 密度过滤伪色块
        draw_info(img, max_blob)

        if should_send():
            # 优化数据协议（增加校验位）
            header = 0xFE
            footer = 0xFD
            data = bytearray([header,
                            max_blob.cx() >> 8, max_blob.cx() & 0xFF,
                            max_blob.cy() >> 8, max_blob.cy() & 0xFF,
                            footer])
            uart.write(data)
            print("Sent: (%d, %d) @%dms" % (max_blob.cx(), max_blob.cy(), utime.ticks_ms()))

    # 显示帧率控制
    lcd.display(img)
    time.sleep_ms(FRAME_DELAY)
