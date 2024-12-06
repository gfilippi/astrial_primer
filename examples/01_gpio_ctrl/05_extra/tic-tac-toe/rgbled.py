import smbus2, time

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

# flatten list of list
def flatten(xss):
        return [x for xs in xss for x in xs]

def display_4x4(data):
        bus = smbus2.SMBus(4)
        addr = 0x6a

        # convert list of list
        tmp = list(flatten(data))
        pattern = list(flatten(data))

        # remap data into a pattern
        for i in range(16):
           pattern[display_mtx[i]]=tmp[i]

        # add API_VER and CMD=2
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

        bus.close()


def main():
    pass


if __name__ == "__main__":
	main()
