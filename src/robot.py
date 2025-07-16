import servo
import asyncio
from dynamixel_python.dynamixel_python import DynamixelManager, DynamixelMotor

DYNAMIXEL_MODEL = "xl330-m288"


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


class DynamixelRobot(DynamixelManager):
    def __init__(self, usb_port, baudrate, motor_ids: list[int] = []):
        super().__init__(usb_port, baud_rate=baudrate)
        self.init()
        self.loop = asyncio.get_event_loop()
        self.id_motors: dict[int, DynamixelMotor] = {}
        for motor_id in motor_ids:
            self.add_motor(motor_id=motor_id)

    def safe_get(self, motor_id, attr, *args, **kwargs):
        return self.loop.run_until_complete(
            self.safe_io(
                motor_id=motor_id, attr=attr, _op="get", *args, **kwargs
            )
        )

    def safe_set(self, motor_id, attr, *args, **kwargs):
        return self.loop.run_until_complete(
            self.safe_io(
                motor_id=motor_id, attr=attr, _op="set", *args, **kwargs
            )
        )

    def add_motor(self, motor_id, motor_model=DYNAMIXEL_MODEL):
        self.id_motors.update(
            {
                motor_id: super().add_dynamixel(
                    f"motor_{motor_id}", motor_id, motor_model
                )
            }
        )

    async def safe_io(
        self, motor_id: int, attr: str, _op: str, n_attempts=4, **kwargs
    ):
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
