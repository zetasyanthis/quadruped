BINS += \
	src/kinematics/inverse_test \
	src/kinematics/forward_test \
	src/kinematics/balance_test
CLEANS += \
	src/kinematics/*.o

src/kinematics/balance_test: src/kinematics/balance_test.o src/kinematics/balance.o src/kinematics/forward.o src/kinematics/inverse.o src/kinematics/kin_helper.o
src/kinematics/forward_test: src/kinematics/forward_test.o src/kinematics/forward.o src/kinematics/kin_helper.o
src/kinematics/inverse_test: src/kinematics/inverse_test.o src/kinematics/inverse.o

src/kinematics/balance.o: src/kinematics/balance.c src/kinematics/kin_helper.h src/kinematics/forward.h src/kinematics/inverse.h src/helper.h
src/kinematics/forward.o: src/kinematics/forward.c src/kinematics/leg_defs.h src/kinematics/kin_helper.h
src/kinematics/inverse.o: src/kinematics/inverse.c src/kinematics/nn_defs.h src/kinematics/leg_defs.h
src/kinematics/kin_helper.o: src/kinematics/kin_helper.c src/kinematics/kin_helper.h

src/kinematics/nn_defs.h: src/kinematics/network.xml src/kinematics/nn_defs.py
	src/kinematics/nn_defs.py $< $@

src/kinematics/leg_defs.h: src/kinematics/leg_defs.py
	$< $@

