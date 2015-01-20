BINS += src/static/static_walk
CLEANS += src/static/*.o


STATIC_MOTION_HEADERS := \
	src/rx28/rs485.h \
	src/rx28/rx28.h \
	src/rx28/rx28_simple.h \
	src/static/static_motion.h

src/static/static_motion.o: src/static/static_motion.c $(STATIC_MOTION_HEADERS)


STATIC_WALK_OBJECTS := \
	src/rx28/rs485.o \
	src/rx28/rx28.o \
	src/rx28/rx28_simple.o \
	src/static/static_motion.o
STATIC_WALK_HEADERS := \
	$(patsubst %.o,%.h,$(STATIC_WALK_OBJECTS)) \
	src/static/static_paths.h

src/static/static_walk.o: src/static/static_walk.c $(STATIC_WALK_HEADERS)

src/static/static_walk: src/static/static_walk.o $(STATIC_WALK_OBJECTS)

