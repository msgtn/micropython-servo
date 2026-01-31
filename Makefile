.PHONY: micropython-dl 
micropython-dl:
	# sudo wget https://micropython.org/download/rp2-pico-w/rp2-pico-w-latest.uf2 -P /media/$(shell whoami)/RPI-RP2
	cp ./uf2/rp2-pico-w-latest.uf2 /media/$(shell whoami)/RPI-RP2/

.PHONY: pico-reset
pico-reset:
	# sudo wget https://datasheets.raspberrypi.com/soft/flash_nuke.uf2 -P /media/$(shell whoami)/RPI-RP21/
	cp ./uf2/flash_nuke.uf2 /media/$(shell whoami)/RPI-RP2/

main:
	cp src/main.py /pyboard

deps:
	cp -r src/dynamixel_python /pyboard/
	cp -r src/servo /pyboard/
	cp -r src/*.py /pyboard/
	mkdir -p /pyboard/control_tables
	cp control_tables/xl330-m288.json /pyboard/control_tables

