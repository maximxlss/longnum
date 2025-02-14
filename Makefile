CXXFLAGS += -g --std=c++23 -pedantic -Wall

BUILD_FOLDER?=build

RELEASE ?= 0
ifeq ($(RELEASE), 0)
	BUILD_FOLDER:=$(BUILD_FOLDER)/debug
else
	BUILD_FOLDER:=$(BUILD_FOLDER)/release
	CFLAGS=-DNDEBUG
	CXXFLAGS+=-O3 -flto
endif

COVERAGE ?= 0
ifeq ($(COVERAGE), 1)
	BUILD_FOLDER:=$(BUILD_FOLDER)/coverage
	CFLAGS=-DNDEBUG
	CXXFLAGS+=-fprofile-arcs -ftest-coverage
	POST_BUILD_COMMAND+=\
		lcov --capture --directory $(BUILD_FOLDER) --output-file $(BUILD_FOLDER)/coverage.info;\
		genhtml $(BUILD_FOLDER)/coverage.info --output-directory $(BUILD_FOLDER)/coverage-report;
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
	$(POST_BUILD_COMMAND)

run.time: $(BUILD_FOLDER)/longnum-bin
	bash -c "time $(BUILD_FOLDER)/longnum-bin"
	$(POST_BUILD_COMMAND)

run.valgrind: $(BUILD_FOLDER)/longnum-bin
	valgrind $(BUILD_FOLDER)/longnum-bin
	$(POST_BUILD_COMMAND)

test: $(BUILD_FOLDER)/tests
	$(BUILD_FOLDER)/tests
	$(POST_BUILD_COMMAND)

test.valgrind: $(BUILD_FOLDER)/tests
	valgrind $(BUILD_FOLDER)/tests
	$(POST_BUILD_COMMAND)

clean:
	rm -rf build
