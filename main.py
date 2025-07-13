import asyncio
import time
import network
import servo
import sys
from dynamixel_python import DynamixelManager, DynamixelMotor

if hasattr(network, "WLAN"):
    # the board has WLAN capabilities
    print("HAs WLAN")


class Robot:
    def __init__(self, motor_pin_ids: dict[int, int] = {}):
        self.motors = {
            motor_id: servo.Servo(pin_id=pin_id)
            for motor_id, pin_id in motor_pin_ids.items()
        }
        self.loop = asyncio.get_event_loop()

    @property
    def motor_states(self):
        return self.read_motor_states()

    def add_servo(self, pin_id):
        self.motors.update({pin_id: servo.Servo(pin_id=pin_id)})

    def read_motor_states(self):
        return self.loop.run_until_complete(self.__async_read_motor_states())

    async def __async_read_motor_states(self):
        state_dict = {}
        for k, v in self.motors.items():
            state_dict.update({k: v.read()})
        return state_dict

    def write_motor_states(self, *args, **kwargs):
        return self.loop.run_until_complete(
            self.__async_write_motor_states(*args, **kwargs)
        )

    async def __async_write_motor_states(self, cmd_dict):
        for k, v in cmd_dict.items():
            if k in self.motors:
                self.motors[k].write(max(min(v, 180), 0))


# robot = Robot({i: pin for i, pin in enumerate([2, 3, 4, 5])})


def cmddict_from_msg(msg: str):
    # input like: 1=50&2=30
    ret = {}
    lines = msg.split("&")
    # print(msg)
    # print("\n")
    for line in lines:
        splits = line.split("=")[:2]
        if len(splits) == 0 or len(splits[0]) == 0:
            continue
        # print(f"{splits=}")
        [pin_id, value] = splits
        ret.update({int(pin_id): float(value)})

    return ret


USB_PORT = "/dev/tty.usbmodem101"
BAUDRATE = 57600
DYNAMIXEL_MODEL = "xl330-m288"
ID = 1


class DynamixelRobot(DynamixelManager):
    def __init__(self, usb_port=USB_PORT, baudrate=BAUDRATE):
        super().__init__(usb_port, baud_rate=baudrate)
        # self.motors = DynamixelManager(usb_port, baud_rate=baudrate)
        # self.motor = self.add_motor(ID, DYNAMIXEL_MODEL)
        self.init()
        self.id_motors: dict[int, DynamixelMotor] = {}
        self.loop = asyncio.get_event_loop()

        # self.safe_set = partial(self.safe_io, _op="set")
        # self.safe_get = partial(self.safe_io, _op="get")

    def safe_get(self, motor_id, attr, *args, **kwargs):
        return self.loop.run_until_complete(
            self.safe_io(motor_id=motor_id, attr=attr, _op="get", *args, **kwargs)
        )

    def safe_set(self, motor_id, attr, *args, **kwargs):
        return self.loop.run_until_complete(
            self.safe_io(motor_id=motor_id, attr=attr, _op="set", *args, **kwargs)
        )

    def add_motor(self, motor_id, motor_model=DYNAMIXEL_MODEL):
        self.id_motors.update(
            {
                motor_id: super().add_dynamixel(
                    f"motor_{motor_id}", motor_id, motor_model
                )
            }
        )

    async def safe_io(self, motor_id: int, attr: str, _op: str, n_attempts=4, **kwargs):
        io_attr_fn = getattr(self.id_motors[motor_id], f"{_op}_{attr}", None)
        if io_attr_fn is not None:
            attempts = 0
            while attempts < n_attempts:
                attempts += 1
                try:
                    ret = io_attr_fn(*args, **kwargs)
                    if (_op == "set" and ret) or _op == "get":
                        break
                except Exception:
                    # time.sleep(0.04)
                    await asyncio.sleep(0.05)
            return ret
        else:
            return None

    def write_motor_states(self, msg_dict: dict[int, float]):
        for motor_id, msg in msg_dict.items():
            if motor_id in self.id_motors:
                self.id_motors[motor_id].set_goal_position(int(msg))
                # max(0, int(msg*4096/180)))
                print(f"wrote {motor_id=} to {msg=}")


# TODO organize dynamixel library in a respective dir
robot = DynamixelRobot()

for i in [1, 2, 3, 4]:
    robot.add_motor(motor_id=i)
    robot.id_motors[i].set_led(True)
    time.sleep(0.2)
    robot.id_motors[i].set_led(False)
    robot.id_motors[i].set_torque_enable(True)

import math as np

while True:
    v = sys.stdin.readline().strip()

    msg_dict = cmddict_from_msg(v)
    # print("received msg\n")
    # print(msg_dict)
    robot.write_motor_states(msg_dict)

# t = 0.0
# del_t = 0.01
# while True:
#     x = int(1024 * np.sin(t) + 1024)
#     msg_dict = {i:x for i in [1,2,3]}
#     robot.write_motor_states(msg_dict)
#     time.sleep(del_t)
#     t += del_t
