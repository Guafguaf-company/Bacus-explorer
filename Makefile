# ==================================================
# BACUS EXPLORER - BUILD SYSTEM
# Guafguaf Company
# ==================================================

PROJECT := BacusExplorer
BUILD_DIR ?= build
CMAKE ?= cmake
CTEST ?= ctest
JOBS ?= 2
EXECUTABLE := $(BUILD_DIR)/bacus-explorer

.PHONY: all configure build test run clean rebuild package install help

# Default target
all: build

# Configure CMake
configure:
	$(CMAKE) -S . -B $(BUILD_DIR) -DBUILD_TESTING=ON

# Compile Bacus Explorer
build: configure
	$(CMAKE) --build $(BUILD_DIR) --parallel $(JOBS)

# Run automated tests
test: build
	$(CTEST) --test-dir $(BUILD_DIR) --output-on-failure

# Launch Bacus Explorer
run: build
	$(EXECUTABLE)

# Remove compiled objects
clean:
	$(CMAKE) --build $(BUILD_DIR) --target clean

# Clean and rebuild
rebuild: clean build

# Packaging will be implemented with CMake install rules
package:
	@echo "Packaging is not configured yet."
	@echo "CMake install rules and a WebKitGTK runtime strategy are required."
	@exit 1

# Installation will be implemented later
install:
	@echo "Installation rules are not configured yet."
	@echo "See the project CMakeLists.txt."

# Show available commands
help:
	@echo ""
	@echo "Bacus Explorer - Build System"
	@echo "============================"
	@echo "make configure  - Configure CMake"
	@echo "make build      - Compile the application"
	@echo "make test       - Build and run tests"
	@echo "make run        - Build and launch Bacus Explorer"
	@echo "make clean      - Remove compiled objects"
	@echo "make rebuild    - Clean and rebuild"
	@echo "make package    - Packaging (not configured yet)"
	@echo "make install    - Installation (not configured yet)"
	@echo "make help       - Show this help"
	@echo ""
