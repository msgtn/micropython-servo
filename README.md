Fork of [`redoxcode/micropython-servo`](https://github.com/redoxcode/micropython-servo) for the Blossom robot, with support for Dynamixel servos.
This software for the Pi Pico microcontroller receives serial commands from an external Python process and controls servo motor positions.

# Usage

## Materials
- Raspberry Pi Pico or Pico W
- Motors, either Dynamixels or simple TowerPro SG90 servos. Follow the guide at `r0b0/docs/blsm.md` for wiring the hardware

## Prereqs

### MicroPython
MicroPython loaded onto the Pico. Follow the official instructions to set the Pico up: https://www.raspberrypi.com/documentation/microcontrollers/micropython.html

### rshell
rshell for MicroPython: https://github.com/dhylands/rshell.
If we have [`uv` installed](https://docs.astral.sh/uv/getting-started/installation/), we can use rshell as a `uvx` tool.
With the Pico plugged in:
```
# use sudo to ensure access to the board
sudo uvx rshell 
```

## Copy files to board
These `make` commands copy the dependencies and main script to the board.
On boot, the board will run `main.py`.
Updates to `main.py` require reuploading the file with `make main`.
```
make deps
make main
```

## repl
Enter the Read-Evaluate-Print-Loop with:
```
repl
```
| Key | Command |
| --- | ------- |
| `Ctrl-X` | Exit |
| `Ctrl-D` | Reboot board |
| `Ctrl-C` | Quit without exiting |

It's advisable to reboot with `Ctrl-D` whenever entering the REPL. 
By default, `main.py` will loop forever, listening on the serial connection for motor commands.
