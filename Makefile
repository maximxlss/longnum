CXXFLAGS += -g --std=c++23 -pedantic -Wall

BUILD_FOLDER?=build

RELEASE ?= 0
ifeq ($(RELEASE), 0)
	BUILD_FOLDER:=$(BUILD_FOLDER)/debug
else
	BUILD_FOLDER:=$(BUILD_FOLDER)/release
	CXXFLAGS+=-O3 -flto -DNDEBUG
endif

COVERAGE ?= 0
ifeq ($(COVERAGE), 1)
	BUILD_FOLDER:=$(BUILD_FOLDER)/coverage
	CXXFLAGS+=-fprofile-arcs -ftest-coverage
	POST_BUILD_COMMAND+=\
		lcov --capture --directory $(BUILD_FOLDER) --output-file $(BUILD_FOLDER)/coverage.info;\
		genhtml $(BUILD_FOLDER)/coverage.info --output-directory $(BUILD_FOLDER)/coverage-report;
endif

COMPILE = $(CXX) $(CXXFLAGS)

.phony: all run run.time run.valgrind run.callgrind test test.valgrind test.callgrind pi pi.time clean

all: $(BUILD_FOLDER)/longnum.o

$(BUILD_FOLDER):
	mkdir -p $(BUILD_FOLDER)

$(BUILD_FOLDER)/longnum.o: src/longnum.cpp src/longnum.hpp | $(BUILD_FOLDER)
	$(COMPILE) $< -c -o $@

$(BUILD_FOLDER)/longnum-bin.o: src/longnum-bin.cpp src/longnum.hpp | $(BUILD_FOLDER)
	$(COMPILE) $< -c -o $@

$(BUILD_FOLDER)/calculate_pi.o: src/calculate_pi.cpp src/longnum.hpp | $(BUILD_FOLDER)
	$(COMPILE) $< -c -o $@

$(BUILD_FOLDER)/tests.o: tests/tests.cpp src/longnum.hpp tests/utils.hpp tests/longnum-tests.cpp | $(BUILD_FOLDER)
	$(COMPILE) $< -c -o $@

$(BUILD_FOLDER)/longnum-bin: $(BUILD_FOLDER)/longnum-bin.o $(BUILD_FOLDER)/longnum.o | $(BUILD_FOLDER)
	$(COMPILE) $^ -o $@

$(BUILD_FOLDER)/calculate_pi: $(BUILD_FOLDER)/calculate_pi.o $(BUILD_FOLDER)/longnum.o | $(BUILD_FOLDER)
	$(COMPILE) $^ -o $@

$(BUILD_FOLDER)/tests: $(BUILD_FOLDER)/tests.o $(BUILD_FOLDER)/longnum.o | $(BUILD_FOLDER)
	$(COMPILE) $^ -o $@

run: $(BUILD_FOLDER)/longnum-bin
	$(BUILD_FOLDER)/longnum-bin
	$(POST_BUILD_COMMAND)

run.time: $(BUILD_FOLDER)/longnum-bin
	bash -c "time $(BUILD_FOLDER)/longnum-bin"
	$(POST_BUILD_COMMAND)

run.valgrind: $(BUILD_FOLDER)/longnum-bin
	valgrind $(BUILD_FOLDER)/longnum-bin
	$(POST_BUILD_COMMAND)

run.callgrind: $(BUILD_FOLDER)/longnum-bin
	valgrind --tool=callgrind --dump-instr=yes --collect-jumps=yes $(BUILD_FOLDER)/longnum-bin
	$(POST_BUILD_COMMAND)

test: $(BUILD_FOLDER)/tests
	$(BUILD_FOLDER)/tests
	$(POST_BUILD_COMMAND)

test.valgrind: $(BUILD_FOLDER)/tests
	valgrind $(BUILD_FOLDER)/tests
	$(POST_BUILD_COMMAND)

test.callgrind: $(BUILD_FOLDER)/tests
	valgrind --tool=callgrind --dump-instr=yes --collect-jumps=yes $(BUILD_FOLDER)/tests
	$(POST_BUILD_COMMAND)

pi: $(BUILD_FOLDER)/calculate_pi
	$(BUILD_FOLDER)/calculate_pi $(DIGITS)
	$(POST_BUILD_COMMAND)

pi.time: $(BUILD_FOLDER)/calculate_pi
	bash -c "time $(BUILD_FOLDER)/calculate_pi $(DIGITS)"
	$(POST_BUILD_COMMAND)

clean:
	rm -rf build
