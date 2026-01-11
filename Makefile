.PHONY: micropython-dl 
micropython-dl:
	sudo wget https://micropython.org/download/rp2-pico-w/rp2-pico-w-latest.uf2 -P /media/$(shell whoami)/RPI-RP2

main:
	cp src/main.py /pyboard

deps:
	rsync src/ /pyboard
