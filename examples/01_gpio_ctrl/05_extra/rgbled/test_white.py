import smbus2, time

# BUS number 4
bus = smbus2.SMBus(4)

# ATTiny I2C address
addr = 0x6a

usleep = lambda x: time.sleep(x/1000000.0)

# API_MJR, API_MIN, CMD (all white)
data = [0,0, 1]

while True:
	bus.write_i2c_block_data(addr, 0, data)

	#usleep(100)
	b = bus.read_byte(addr)
	print(b)

	usleep(80000)

bus.close()
