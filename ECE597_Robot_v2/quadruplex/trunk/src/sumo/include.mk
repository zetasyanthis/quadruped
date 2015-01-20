BINS += \
	src/sumo/sumo_demo
CLEANS += \
	src/control/*.o

src/sumo/omap3530-pwm.o: src/sumo/omap3530-pwm.c src/sumo/omap3530-pwm.h

SUMO_DEMO_OBJECTS := \
	src/sumo/omap3530-pwm.o \
	src/sensors/i2c_bus.o \
	src/sensors/srf08.o
SUMO_DEMO_HEADERS := $(patsubst %.o,%.h,$(DEMO_OBJECTS))

src/sumo/sumo_demo: src/sumo/sumo_demo.o $(SUMO_DEMO_OBJECTS)
src/sumo/sumo_demo.o: src/sumo/sumo_demo.c $(SUMO_DEMO_HEADERS)

