import math as np
import sys
import time
from utime import sleep_us
import select

from machine import UART, Pin

from robot import DynamixelRobot, Robot


led = Pin("LED", Pin.OUT)

USB_PORT = "/dev/tty.usbmodem101"
BAUDRATE = 57600

robot = DynamixelRobot(
    usb_port=USB_PORT, baudrate=BAUDRATE, motor_ids=[1, 2, 3, 4]
)
# robot = Robot(motor_pin_ids={1: 2, 2: 3, 3: 6, 4: 7})

uart = UART(1, 115200, tx=Pin(4), rx=Pin(5), timeout_char=100)
poll = select.poll()
poll.register(sys.stdin)

# serial = UART(0, 115200, timeout_char=100)

from hcsr04 import HCSR04

# pin_trigger = 14
# pin_echo = 15
# sensor = HCSR04(
#     trigger_pin=pin_trigger,
#     echo_pin=pin_echo,
# )
PORT = "/dev/ttyACM0"


def cmddict_from_msg(msg: str):
    # input like: 1=50&2=30
    ret = {}
    lines = msg.split("&")
    for line in lines:
        splits = line.split("=")[:2]
        if len(splits) == 0 or len(splits[0]) == 0:
            continue
        # print(splits)
        [pin_id, value] = splits
        ret.update({int(pin_id): float(value)})

    return ret


def read_stdin():
    v = sys.stdin.readline().strip()
    msg_dict = cmddict_from_msg(v)
    robot.write_motor_states(msg_dict)


def read_cmds():
    while True:
        # Check if there is data available on sys.stdin
        if select.select([sys.stdin], [], [], 0.0)[0]:
            v = sys.stdin.readline().strip()
            msg_dict = cmddict_from_msg(v)
            robot.write_motor_states(msg_dict)
        else:
            # Add a small delay to avoid busy-waiting
            sleep_us(5)


def read_uart(buffer):
    # Read available data into the buffer
    buffer_index = 0
    try:
        while uart.any():
            data = uart.read(1)  # Read one byte at a time
            if data:
                if buffer_index < len(buffer):
                    buffer[buffer_index] = data[0]
                    buffer_index += 1

                # Check for a newline character to process a complete message
                if data == b"\n":
                    # Decode the message and process it
                    v = buffer[:buffer_index].decode("utf-8").strip()
                    msg_dict = cmddict_from_msg(v)
                    robot.write_motor_states(msg_dict)

                    # Reset the buffer index for the next message
                    # buffer_index = 0
        v = buffer[:buffer_index].decode("utf-8").strip()
    except:
        pass


def read_cmds_uart():
    buffer = bytearray(1024)  # Pre-allocate a buffer of length 1024
    last_check_time = time.ticks_ms()
    check_interval_ms = 2000  # Check one motor every 2 seconds
    motor_check_index = 0  # Which motor to check next

    while True:
        led.value(0)
        if uart.any() > 0:
            led.value(1)
            read_uart(buffer)

        elif select.select([sys.stdin], [], [], 0.0)[0]:
            led.value(1)
            read_stdin()

        # Check one motor at a time to avoid blocking
        now = time.ticks_ms()
        if time.ticks_diff(now, last_check_time) >= check_interval_ms:
            last_check_time = now
            motor_ids = robot.motor_ids
            if motor_ids:
                motor_id = motor_ids[motor_check_index % len(motor_ids)]
                motor_check_index += 1
                if not robot.check_motor(motor_id):
                    robot.reconnect_motor(motor_id)

        # Add a small delay to avoid busy-waiting
        sleep_us(10)


def sin_mvmt():
    t = 0.0
    del_t = 0.01
    while True:
        x = int(1024 * np.sin(t) + 1024)
        msg_dict = {i: x for i in [1, 2, 3]}
        robot.write_motor_states(msg_dict)
        time.sleep(del_t)
        t += del_t


# def all_to_val(value=90):
#     robot.write_motor_states


# NOTE: select which of these functions to run
# read_cmds()
read_cmds_uart()
# robot.write_motor_states({i: 20 for i in [1,2,3]})
# time.sleep(1)
# robot.write_motor_states({i: 90 for i in [1
