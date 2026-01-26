dxl:
	cmake -B build -DUSE_DYNAMIXEL=ON -DUSE_DISTANCE_SENSOR=OFF .

servo:
	cmake -B build .

flash:
	sudo picotool load -f build/servo_controller.uf2
	sudo picotool reboot -f
