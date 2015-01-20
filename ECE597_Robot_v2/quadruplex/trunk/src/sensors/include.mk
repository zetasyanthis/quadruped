BINS += \
	src/sensors/i2c_bus_test \
	src/sensors/sensor_test
CLEANS += \
	src/sensors/*.o

src/sensors/sensor_test: src/sensors/sensor_test.o


src/sensors/sensor_test.o: src/sensors/sensor_test.c
src/sensors/AD7991.o: src/sensors/AD7991.c

src/sensors/i2c_bus.o: src/sensors/i2c_bus.c src/sensors/i2c_bus.h

I2C_BUS_TEST_OBJECTS := \
	src/sensors/i2c_bus.o \
	src/sensors/srf08.o \
	src/sensors/AD7991.o
src/sensors/i2c_bus_test: src/sensors/i2c_bus_test.o $(I2C_BUS_TEST_OBJECTS)
src/sensors/i2c_bus_test.o: src/sensors/i2c_bus_test.c $(patsubst .o,.h,$(I2C_BUS_TEST_OBJECTS))

src/sensors/srf08.o: src/sensors/srf08.c src/sensors/srf08.h src/sensors/i2c_bus.h
src/sensors/AD7991.o: src/sensors/AD7991.c src/sensors/AD7991.h src/sensors/i2c_bus.h
