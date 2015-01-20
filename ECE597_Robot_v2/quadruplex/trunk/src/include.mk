BINS += \
	src/demo \
	src/path_demo
CLEANS += \
	src/demo.o \
	src/path_demo.o

DEMO_OBJECTS := \
	src/rx28/rs485.o \
	src/rx28/rx28.o \
	src/rx28/rx28_simple.o \
	src/kinematics/inverse.o \
	src/kinematics/forward.o \
	src/kinematics/kin_helper.o \
	src/kinematics/balance.o \
	src/control/wiimote.o
DEMO_HEADERS := \
	src/helper.h \
	$(patsubst %.o,%.h,$(DEMO_OBJECTS))

src/demo.o: src/demo.c $(DEMO_HEADERS)

src/demo: src/demo.o $(DEMO_OBJECTS)


PATH_DEMO_OBJECTS := \
	src/rx28/rs485.o \
	src/rx28/rx28.o \
	src/rx28/rx28_simple.o \
	src/kinematics/inverse.o \
	src/kinematics/forward.o \
	src/kinematics/kin_helper.o \
	src/kinematics/balance.o \
	src/control/wiimote.o \
	src/sensors/i2c_bus.o	\
	src/sensors/srf08.o
PATH_DEMO_HEADERS := \
	src/helper.h \
	$(patsubst %.o,%.h,$(PATH_DEMO_OBJECTS))

src/path_demo.o: src/path_demo.c $(PATH_DEMO_HEADERS)

src/path_demo: src/path_demo.o $(PATH_DEMO_OBJECTS)

