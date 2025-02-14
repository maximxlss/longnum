CXXFLAGS += -g --std=c++23 -pedantic -Wall
test.coverage: CXXFLAGS += -fprofile-arcs -ftest-coverage

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	BUILD_FOLDER=build/debug
else
	BUILD_FOLDER=build/release
	CFLAGS=-DNDEBUG
	CXXFLAGS += -O3 -flto
endif

COMPILE = $(CXX) $(CXXFLAGS)

.phony: all run run.time run.valgrind test test.valgrind clean

all: $(BUILD_FOLDER)/longnum.o

$(BUILD_FOLDER):
	mkdir -p $(BUILD_FOLDER)

$(BUILD_FOLDER)/longnum.o: src/longnum.cpp src/longnum.hpp | $(BUILD_FOLDER)
	$(COMPILE) $< -c -o $@

$(BUILD_FOLDER)/longnum-bin.o: src/longnum-bin.cpp | $(BUILD_FOLDER)
	$(COMPILE) $< -c -o $@

$(BUILD_FOLDER)/tests.o: tests/tests.cpp tests/utils.hpp tests/longnum-tests.cpp | $(BUILD_FOLDER)
	$(COMPILE) $< -c -o $@

$(BUILD_FOLDER)/longnum-bin: $(BUILD_FOLDER)/longnum-bin.o $(BUILD_FOLDER)/longnum.o | $(BUILD_FOLDER)
	$(COMPILE) $^ -o $@

$(BUILD_FOLDER)/tests: $(BUILD_FOLDER)/tests.o $(BUILD_FOLDER)/longnum.o | $(BUILD_FOLDER)
	$(COMPILE) $^ -o $@

run: $(BUILD_FOLDER)/longnum-bin
	$(BUILD_FOLDER)/longnum-bin

run.time: $(BUILD_FOLDER)/longnum-bin
	bash -c "time $(BUILD_FOLDER)/longnum-bin"

run.valgrind: $(BUILD_FOLDER)/longnum-bin
	valgrind $(BUILD_FOLDER)/longnum-bin

test: $(BUILD_FOLDER)/tests
	$(BUILD_FOLDER)/tests

test.valgrind: $(BUILD_FOLDER)/tests
	valgrind $(BUILD_FOLDER)/tests

test.coverage: $(BUILD_FOLDER)/tests
	$(BUILD_FOLDER)/tests
	lcov --capture --directory $(BUILD_FOLDER) --output-file $(BUILD_FOLDER)/coverage.info
	genhtml $(BUILD_FOLDER)/coverage.info --output-directory $(BUILD_FOLDER)/coverage-report

clean:
	rm -rf build
