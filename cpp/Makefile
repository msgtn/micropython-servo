dxl:
	cmake -B build -DUSE_DYNAMIXEL=ON -DUSE_DISTANCE_SENSOR=OFF .
	cd build && make

servo:
	cmake -B build .
	cd build && make

flash:
	sudo picotool load -f build/servo_controller.uf2
	sudo picotool reboot -f
