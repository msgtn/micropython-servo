import serial
import time

# open a serial connection
s = serial.Serial("/dev/ttyACM0", 115200)

# blink the led
while True:
    value = float(input('value: '))
    s.write(bytes(f"1={value}\n", encoding="utf-8"))
    # s.write(b"1={input('value: ')}\n")
    # time.sleep(1)
    # s.write(b"1=160\n")
    # time.sleep(1)
