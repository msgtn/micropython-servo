main:
	cp main.py /pyboard

deps:
	cp -r src/dynamixel_python /pyboard/
	cp -r src/servo /pyboard/
	cp -r src/*.py /pyboard/
	mkdir -p /pyboard/control_tables
	cp control_tables/xl330-m288.json /pyboard/control_tables
