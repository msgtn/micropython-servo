import serial


ser = serial.Serial("/dev/ttyACM0", 115200, timeout=1)

try:
    while True:
        line = ser.readline()
        if line:
            print(line.decode("utf-8").strip())
except KeyboardInterrupt:
    ser.close()
