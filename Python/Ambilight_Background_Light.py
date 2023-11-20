import pyautogui
from PIL import ImageGrab
import numpy as np
import time
import socket

# 获取屏幕分辨率
screen_width, screen_height = pyautogui.size()

# 边缘宽度（可以根据需要修改）
border_size = 1

# 采样点数量
num_samples = 150

# 计算采样间隔，确保将顶部边缘分成num_samples份
sample_interval = (screen_width // num_samples)+1;

# TCP服务器的主机和端口
server_host = '10.0.1.227'  # 服务器地址
server_port = 5000  # 服务器端口号

# 截取屏幕顶部边缘的像素颜色并存储在数组中
def get_edge_colors(screen_width, screen_height, border_size, num_samples):
    # 使用ImageGrab.grab()函数获取整个屏幕的截图
    screen = ImageGrab.grab()

    # 截取屏幕上顶部边缘区域的部分
    top_edge = screen.crop((0, 0, screen_width, border_size))

    # 将顶部边缘区域转换为numpy数组
    top_edge_pixels = np.array(top_edge)

    # 从顶部边缘区域中采样一个颜色值
    edge_colors = []
    for x in range(0, screen_width, sample_interval):
        pixel = top_edge_pixels[0][x]
        edge_colors.append(pixel)

    return edge_colors

# 创建TCP客户端socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# 连接到TCP服务器
client_socket.connect((server_host, server_port))

try:
    while True:
        edge_colors = get_edge_colors(screen_width, screen_height, border_size, num_samples)
        print("screen_width:")
        print(screen_width)
        print("\r\nnum_samples:")
        print(num_samples)
        print("\r\nsample_interval:")
        print(sample_interval)
        # 打印采样点的颜色
        for i, color in enumerate(edge_colors):
            print(f"Sample {i + 1}: {color}")

        # 将数据转换为16进制字符串，并发送到服务器
        data_bytes = b"".join([bytes(color) for color in edge_colors])
        client_socket.send(data_bytes)

        # 休眠0.1秒（100毫秒）
        time.sleep(0.2)

except KeyboardInterrupt:
    print("Program terminated by user.")

finally:
    # 关闭TCP连接
    client_socket.close()
