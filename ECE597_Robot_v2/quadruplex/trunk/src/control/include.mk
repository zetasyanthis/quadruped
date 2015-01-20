BINS += \
	src/control/wiimote_test
CLEANS += \
	src/control/*.o

src/control/wiimote.o: src/control/wiimote.h
src/control/wiimote_test.o: src/control/wiimote.h
src/control/wiimote_test: src/control/wiimote_test.o src/control/wiimote.o

