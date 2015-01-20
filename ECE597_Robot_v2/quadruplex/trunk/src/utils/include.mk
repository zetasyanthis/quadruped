BINS += \
	src/utils/get_motor_pos \
	src/utils/get_motor_angles \
	src/utils/stand_symmetric \
	src/utils/reset_motors \
	src/utils/ping_motors
CLEANS += src/utils/*.o


GET_MOTOR_POS_OBJECTS := \
	src/rx28/rs485.o \
	src/rx28/rx28.o \
	src/rx28/rx28_simple.o \
	src/kinematics/forward.o \
	src/kinematics/kin_helper.o
GET_MOTOR_POS_HEADERS := \
	src/helper.h \
	$(patsubst %.o,%.h,$(GET_MOTOR_POS_OBJECTS))

src/utils/get_motor_pos.o: src/utils/get_motor_pos.c $(GET_MOTOR_POS_HEADERS)

src/utils/get_motor_pos: src/utils/get_motor_pos.o $(GET_MOTOR_POS_OBJECTS)


GET_MOTOR_ANGLES_OBJECTS := \
	src/rx28/rs485.o \
	src/rx28/rx28.o \
	src/rx28/rx28_simple.o
GET_MOTOR_ANGLES_HEADERS := \
	src/helper.h \
	$(patsubst %.o,%.h,$(GET_MOTOR_ANGLES_OBJECTS))

src/utils/get_motor_angles.o: src/utils/get_motor_angles.c $(GET_MOTOR_ANGLES_HEADERS)

src/utils/get_motor_angles: src/utils/get_motor_angles.o $(GET_MOTOR_ANGLES_OBJECTS)


STAND_SYMMETRIC_OBJECTS := \
	src/rx28/rs485.o \
	src/rx28/rx28.o \
	src/rx28/rx28_simple.o \
	src/kinematics/inverse.o
STAND_SYMMETRIC_HEADERS := \
	src/helper.h \
	$(patsubst %.h,%.o,$(STAND_SYMMETRIC_OBJECTS))

src/utils/stand_symmetric.o: src/utils/stand_symmetric.c $(STAND_SYMMETRIC_HEADERS)

src/utils/stand_symmetric: src/utils/stand_symmetric.o $(STAND_SYMMETRIC_OBJECTS)


RESET_MOTORS_OBJECTS := \
	src/rx28/rs485.o \
	src/rx28/rx28.o \
	src/rx28/rx28_simple.o
RESET_MOTORS_HEADERS := \
	src/helper.h \
	$(patsubst %.h,%.o,$(RESET_MOTORS_OBJECTS))

src/utils/reset_motors.o: src/utils/reset_motors.c $(RESET_MOTORS_HEADERS)

src/utils/reset_motors: src/utils/reset_motors.o $(RESET_MOTORS_OBJECTS)

PING_MOTORS_OBJECTS := \
	src/rx28/rs485.o \
	src/rx28/rx28.o
PING_MOTORS_HEADERS := \
	src/helper.h \
	$(patsubst %.h,%.o,$(PING_MOTORS_OBJECTS))

src/utils/ping_motors.o: src/utils/ping_motors.c $(PING_MOTORS_HEADERS)

src/utils/ping_motors: src/utils/ping_motors.o $(PING_MOTORS_OBJECTS)
