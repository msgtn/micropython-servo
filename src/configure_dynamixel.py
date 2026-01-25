"""
Configure a Dynamixel XL330 motor to ID 3 and baudrate 57600.

Uses the existing dynamixel_python module.
"""

import time
from dynamixel_python.dynamixel_python import DynamixelManager

# Target configuration (as expected by main.py)
TARGET_ID = 3
TARGET_BAUDRATE = 57600
MOTOR_MODEL = "xl330-m288"

# Source motor settings (change these if your motor has different settings)
SOURCE_ID = 1
SOURCE_BAUDRATE = 57600


def main():
    print("Dynamixel XL330 Configuration Script")
    print("=" * 40)
    print(f"Source: ID={SOURCE_ID}, Baudrate={SOURCE_BAUDRATE}")
    print(f"Target: ID={TARGET_ID}, Baudrate={TARGET_BAUDRATE}")
    print()

    # Connect to motor at known settings
    print(f"Connecting at {SOURCE_BAUDRATE} baud...")
    manager = DynamixelManager(usb_port="/dev/ttyUSB0", baud_rate=SOURCE_BAUDRATE)
    if not manager.init():
        print("Failed to init port")
        return

    motor = manager.add_dynamixel("motor", SOURCE_ID, MOTOR_MODEL)

    # Blink LED to confirm connection
    print(f"Blinking LED on ID {SOURCE_ID} to confirm connection...")
    print("(Watch for LED blink on motor)")
    for _ in range(3):
        motor.set_led(1)
        time.sleep(0.2)
        motor.set_led(0)
        time.sleep(0.2)

    # Disable torque for EEPROM writes
    print("\nDisabling torque...")
    motor.set_torque_enable(False)
    time.sleep(0.1)

    # Change ID if needed
    if SOURCE_ID != TARGET_ID:
        print(f"Setting ID: {SOURCE_ID} -> {TARGET_ID}...")
        motor.set_id(TARGET_ID)
        time.sleep(0.2)
        print("  Done")

    # Reconnect with new ID
    manager.close()
    manager = DynamixelManager(usb_port="/dev/ttyUSB0", baud_rate=TARGET_BAUDRATE)
    manager.init()
    motor = manager.add_dynamixel("motor", TARGET_ID, MOTOR_MODEL)

    # Blink LED to confirm new ID works
    print(f"\nBlinking LED on ID {TARGET_ID} to confirm new ID...")
    for _ in range(3):
        motor.set_led(1)
        time.sleep(0.2)
        motor.set_led(0)
        time.sleep(0.2)

    # Sweep test
    print("\nRunning sweep test (1024 -> 3072 -> 2048)...")
    print("Enabling torque...")
    motor.set_torque_enable(True)
    time.sleep(0.2)

    print("Moving to position 1024...")
    motor.set_goal_position(1024)
    time.sleep(1.0)

    print("Moving to position 3072...")
    motor.set_goal_position(3072)
    time.sleep(1.0)

    print("Moving to position 2048 (center)...")
    motor.set_goal_position(2048)
    time.sleep(1.0)

    manager.close()
    print("\nConfiguration complete!")
    print(f"Motor should now be at ID={TARGET_ID}, Baudrate={TARGET_BAUDRATE}")


if __name__ == "__main__":
    main()
