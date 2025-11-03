import math as np
import sys
import time
from utime import sleep_us
import select

from machine import UART, Pin

from robot import DynamixelRobot, Robot
from hcsr04 import HCSR04


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

pin_trigger = 14
pin_echo = 15
sensor = HCSR04(
    trigger_pin=pin_trigger,
    echo_pin=pin_echo,
)
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
    last_distance_time = time.ticks_ms()
    distance_interval_ms = 100  # Output every 100ms

    while True:
        # sys.stdout.write(str(sensor.distance_mm()) + "\n")
        # serial.write(str(sensor.distance_mm()) + "\n")

        now = time.ticks_ms()
        if time.ticks_diff(now, last_distance_time) >= distance_interval_ms:
            try:
                sys.stdout.write(str(sensor.distance_mm()) + "\n")
            except Exception as e:
                sys.stdout.write("sensor error\n")
            last_distance_time = now

        led.value(0)
        if uart.any() > 0:
            # print("uart")
            led.value(1)
            read_uart(buffer)

        elif select.select([sys.stdin], [], [], 0.0)[0]:
            # elif poll.poll(1):
            # elif len(stdin_poll) > 0:
            # print("stdin")
            led.value(1)
            read_stdin()
        # Add a small delay to avoid busy-waiting
        sleep_us(10)
        # time.sleep(0.01)


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
