BINS += \
	src/rx28/set_limits \
	src/rx28/torque_off
CLEANS += \
	src/rx28/*.o

RX28_COMMON := \
	src/rx28/rx28.o \
	src/rx28/rs485.o \
	src/rx28/rx28_simple.o

src/rx28/torque_off: src/rx28/torque_off.o $(RX28_COMMON)
src/rx28/set_limits: src/rx28/set_limits.o $(RX28_COMMON)


