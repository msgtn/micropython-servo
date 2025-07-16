import math as np
import sys
import time


from robot import DynamixelRobot, Robot


def cmddict_from_msg(msg: str):
    # input like: 1=50&2=30
    ret = {}
    lines = msg.split("&")
    for line in lines:
        splits = line.split("=")[:2]
        if len(splits) == 0 or len(splits[0]) == 0:
            continue
        [pin_id, value] = splits
        ret.update({int(pin_id): float(value)})

    return ret


USB_PORT = "/dev/tty.usbmodem101"
BAUDRATE = 57600

# TODO organize dynamixel library in a respective dir
robot = DynamixelRobot(
    usb_port=USB_PORT, baudrate=BAUDRATE, motor_ids=[1, 2, 3, 4]
)


def read_cmds():
    while True:
        v = sys.stdin.readline().strip()
        msg_dict = cmddict_from_msg(v)
        robot.write_motor_states(msg_dict)


def sin_mvmt():
    t = 0.0
    del_t = 0.01
    while True:
        x = int(1024 * np.sin(t) + 1024)
        msg_dict = {i: x for i in [1, 2, 3]}
        robot.write_motor_states(msg_dict)
        time.sleep(del_t)
        t += del_t

# NOTE: select which of these functions to run
read_cmds()