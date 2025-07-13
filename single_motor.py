import time
from dynamixel_python import DynamixelManager

USB_PORT = '/dev/tty.usbmodem101'

BAUDRATE = 9600
DYNAMIXEL_MODEL = 'xl330-m288'
ID = 1

# def single_motor_example():
if True:
    """
    turn on a single dynamixel and sweep it between position 0 and position 1024 three times
    """
    motors = DynamixelManager(USB_PORT, baud_rate=BAUDRATE)
    testMotor = motors.add_dynamixel('TestMotor', ID, DYNAMIXEL_MODEL)
    motors.init()

    if not testMotor.ping():
        raise BaseException('motor not configured correctly')

    testMotor.set_operating_mode(3)   
    testMotor.set_led(True)
    testMotor.set_torque_enable(True)
    testMotor.set_profile_velocity(262)

    for i in range(3):
        testMotor.set_goal_position(0)
        time.sleep(0.5)

        testMotor.set_goal_position(1024)
        time.sleep(0.5)

    # testMotor.set_torque_enable(False)
    testMotor.set_led(False)


# if __name__ == '__main__':
    # single_motor_example()
