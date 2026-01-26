"""
Configure a Dynamixel XL330 motor to ID 3 and baudrate 57600.

Uses the existing dynamixel_python module.
Scans for motors using LED blink (since ping/reads don't work reliably).
"""

import gc
import time
from dynamixel_python.dynamixel_python import (
    DynamixelManager,
    get_control_table,
)


def with_retry(fn, attempts=10, delay=0.1):
    """Call fn with retry logic. Returns None if all attempts fail."""
    for _ in range(attempts):
        try:
            return fn()
        except:
            time.sleep(delay)
    return None


# Target configuration (as expected by main.py)
TARGET_ID = 3
TARGET_BAUDRATE = 57600
MOTOR_MODEL = "xl330-m288"

# Baudrates to scan
SCAN_BAUDRATES = [57600, 1000000, 115200, 9600]

# IDs to scan
SCAN_IDS = list(range(1, 20))

# Load control table once to save memory
CTRL_TABLE = get_control_table(MOTOR_MODEL)


def find_motor(manager):
    """
    Scan for a motor by trying to read its present position.
    Returns the motor ID if found, None otherwise.
    """
    for dxl_id in SCAN_IDS:
        motor = manager.add_dynamixel_with_ctrl_table(
            f"motor", dxl_id, CTRL_TABLE
        )
        pos = with_retry(motor.get_present_position)
        if pos is not None and pos >= 0:
            print(f"  -> Found motor at ID {dxl_id} (position: {pos})")
            return dxl_id, motor
        # Clear to free memory
        manager.dxl_dict.clear()
        gc.collect()
    return None, None


def main():
    print("Dynamixel XL330 Configuration Script")
    print("=" * 40)
    print(f"Target: ID={TARGET_ID}, Baudrate={TARGET_BAUDRATE}")
    print()
    print("Scanning for motor (watch for LED blink)...")
    print()

    found_id = None
    found_baudrate = None
    motor = None
    manager = None

    # Scan at different baudrates
    for baudrate in SCAN_BAUDRATES:
        print(f"Trying {baudrate} baud...")
        manager = DynamixelManager(usb_port="/dev/ttyUSB0", baud_rate=baudrate)
        if not manager.init():
            print("  Failed to init port")
            continue

        found_id, motor = find_motor(manager)
        if found_id is not None:
            found_baudrate = baudrate
            break
        manager.close()

    if found_id is None:
        print("\nNo motor found. Check connections and power.")
        return

    print(f"\nMotor found: ID={found_id}, Baudrate={found_baudrate}")

    # Check if already configured
    if found_id == TARGET_ID and found_baudrate == TARGET_BAUDRATE:
        print("Motor already at target configuration!")
    else:
        # Disable torque for EEPROM writes
        print("\nDisabling torque...")
        motor.set_torque_enable(False)
        time.sleep(0.1)

        # Change ID if needed
        if found_id != TARGET_ID:
            print(f"Setting ID: {found_id} -> {TARGET_ID}...")
            # Use write1ByteTxOnly to avoid waiting for response (which times out)
            ADDR_ID = 7
            manager.packetHandler.write1ByteTxOnly(
                manager.portHandler, found_id, ADDR_ID, TARGET_ID
            )
            time.sleep(0.5)
            print("  Done")

        # Reconnect with new ID
        manager.close()
        manager = DynamixelManager(
            usb_port="/dev/ttyUSB0", baud_rate=TARGET_BAUDRATE
        )
        manager.init()

    # Get motor at target ID
    motor = manager.add_dynamixel_with_ctrl_table(
        "motor", TARGET_ID, CTRL_TABLE
    )

    # Blink LED to confirm new ID works
    print(f"\nBlinking LED on ID {TARGET_ID} to confirm...")
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
    print(f"Motor is now at ID={TARGET_ID}, Baudrate={TARGET_BAUDRATE}")


if __name__ == "__main__":
    main()
