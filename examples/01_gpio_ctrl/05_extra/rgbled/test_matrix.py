import smbus2, time

bus = smbus2.SMBus(4)
addr = 0x6a

usleep = lambda x: time.sleep(x/1000000.0)

#
# the LED are a sequential string, we need
# a mapping function to properly display the
# user matrix "aligned"
#
display_mtx=[   3,  4,  11,  12,
		2, 5,  10,  13,
		1, 6,   9,  14,
                0, 7,   8,  15 ]

#
# user data to display
#
data = [1,0,0,2,
        0,1,2,0,
        0,2,1,0,
        2,0,0,1]

while True:
	# remap data into a pattern
	pattern=list(data)
	for i in range(16):
           pattern[display_mtx[i]]=data[i]

	# add API_VER=0,0 and CMD=2 (pattern)
	pattern = [0,0,2] + pattern

	# write to I2C
	rv = False
	while not rv:
		try:
			bus.write_i2c_block_data(addr, 0, pattern)
			rv=True
		except:
			usleep(100)
			pass

	rv=False
	while not rv:
		try:
			b = bus.read_byte(addr)
			rv=True
			print(b)
		except:
			usleep(100)
			pass


	time.sleep(0.08)

bus.close()
