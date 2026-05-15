.PHONY: help configure build test run-phase1 run-phase2 configure-tsan build-tsan test-tsan run-tsan rebuild rebuild-tsan rebuild-all clean

BUILD_DIR := build
TSAN_BUILD_DIR := build-tsan
BUILD_TYPE := Debug

ITEMS ?= 100000
WORK ?= 100
CONSUMERS ?= 4
BURST_SIZE ?= 1
PRODUCER_DELAY_US ?= 0

help:
	@echo "Available commands:"
	@echo "  make configure      Configure normal build"
	@echo "  make build          Build normal targets"
	@echo "  make test           Run normal tests"
	@echo "  make run-phase1     Run Phase 1 demo"
	@echo "  make run-phase2     Run Phase 2 benchmark"
	@echo "  make build-tsan     Configure and build ThreadSanitizer targets"
	@echo "  make test-tsan      Run ThreadSanitizer tests"
	@echo "  make run-tsan       Run Phase 2 benchmark with ThreadSanitizer"
	@echo "  make rebuild        Remove build dirs, then build normal targets"
	@echo "  make rebuild-tsan   Remove build dirs, then build ThreadSanitizer targets"
	@echo "  make rebuild-all    Remove build dirs, then build normal and ThreadSanitizer targets"
	@echo "  make clean          Remove build dirs"
	@echo ""
	@echo "Phase 2 variables:"
	@echo "  ITEMS=100000 WORK=100 CONSUMERS=4 BURST_SIZE=1 PRODUCER_DELAY_US=0"

configure:
	cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

build: configure
	cmake --build $(BUILD_DIR)

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

run-phase1: build
	./$(BUILD_DIR)/video_labs_phase1

run-phase2: build
	./$(BUILD_DIR)/video_labs_phase2 --items $(ITEMS) --consumer-work $(WORK) --consumers $(CONSUMERS) --burst-size $(BURST_SIZE) --producer-delay-us $(PRODUCER_DELAY_US)

configure-tsan:
	cmake -B $(TSAN_BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DSANITIZE_THREAD=ON

build-tsan: configure-tsan
	cmake --build $(TSAN_BUILD_DIR)

test-tsan: build-tsan
	ctest --test-dir $(TSAN_BUILD_DIR) --output-on-failure

run-tsan: build-tsan
	./$(TSAN_BUILD_DIR)/video_labs_phase2 --items $(ITEMS) --consumer-work $(WORK) --consumers $(CONSUMERS) --burst-size $(BURST_SIZE) --producer-delay-us $(PRODUCER_DELAY_US)

rebuild: clean build

rebuild-tsan: clean build-tsan

rebuild-all: clean build build-tsan

clean:
	rm -rf $(BUILD_DIR) $(TSAN_BUILD_DIR)
