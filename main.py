import asyncio
import network
import servo
import sys
from dynamixel_python import DynamixelManager

if hasattr(network, "WLAN"):
    # the board has WLAN capabilities
    print("HAs WLAN")


class Robot:
    def __init__(self, motor_pin_ids: dict[int, int] = {}):
        self.motors = {motor_id: servo.Servo(
            pin_id=pin_id) for motor_id, pin_id in motor_pin_ids.items()}
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
            state_dict.update({
                k: v.read()
            })
        return state_dict

    def write_motor_states(self, *args, **kwargs):
        return self.loop.run_until_complete(self.__async_write_motor_states(*args, **kwargs))

    async def __async_write_motor_states(self, cmd_dict):
        for k, v in cmd_dict.items():
            if k in self.motors:
                self.motors[k].write(max(min(v, 180), 0))


robot = Robot({i: pin for i, pin in enumerate([2, 3, 4, 5])})


def cmddict_from_msg(msg: str):
    # input like: 1=50&2=30
    ret = {}
    lines = msg.split('&')
    print(msg)
    print("\n")
    for line in lines:
        splits = line.split('=')[:2]
        if len(splits) == 0 or len(splits[0])==0:
            continue
        print(f"{splits=}")
        [pin_id, value] = splits
        ret.update({int(pin_id): float(value)})

    return ret


USB_PORT = "/dev/tty.usbmodem101"
BAUDRATE = 9600
DYNAMIXEL_MODEL = "xl330-m288"
ID = 1


class DynamixelRobot:

    def __init__(self, usb_port=USB_PORT, baudrate=BAUDRATE):
        self.motors = DynamixelManager(usb_port, baud_rate=baudrate)
        self.motor = self.motors.add_dynamixel("motor", ID, DYNAMIXEL_MODEL)
        self.motors.init()
        # self.safe_set = partial(self.safe_io, _op="set")
        # self.safe_get = partial(self.safe_io, _op="get")

    async def safe_io(self, attr, _op="get", n_attempts=4, *args, **kwargs):
        io_attr_fn = getattr(self.motor, f"{_op}_{attr}", None)
        if io_attr_fn is not None:
            attempts = 0
            while attempts < n_attempts:
                try:
                    ret = io_attr_fn(*args, **kwargs)
                    if (_op == "set" and ret) or _op == "get":
                        break
                except:

                    await asyncio.sleep(0.04)
            return ret
        else:
            return None


while True:
    v = sys.stdin.readline().strip()

    msg_dict = cmddict_from_msg(v)
    print("received msg\n")
    # print(msg_dict)
    robot.write_motor_states(msg_dict)
