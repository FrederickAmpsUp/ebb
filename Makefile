CC=clang++         # C++ compiler to use
COMPILE_ARGS=-c    # Arguments to compile libraries
CCARGS=-g -Wno-c++17-extensions	-Wno-dangling-gsl	       # Additional arguments (optimizations, debug, warnings, etc)
LDARGS=-lGL -lglfw -lglad -lGLU # Extra libraries
INCLUDE_PATH=/home/frederickampsup/Documents/ebb/include  # Path to include directory  \
														[TODO: figure out how to make clang++ like relative include paths]

        # Base project path
BASE_PATH=/home/frederickampsup/Documents/ebb/

        # Directory containing source files
SOURCE_DIR=src/
        # Directory to put output files
LIB_DIR=lib/
        # Where to put the final output library (relative to LIB_DIR)
LIB_FINAL=libebb.a

        # Directory where tests are located
TEST_DIR=test/
        # Directory for binaries
BIN_DIR=bin/

        # List of test source files
TEST_SOURCES=$(shell find $(TEST_DIR) -type f -name "*.cpp")
        # List of test output files
TEST_OUTS =$(patsubst %.cpp,%,$(TEST_SOURCES))
TEST_OUTS:=$(patsubst $(TEST_DIR)%,$(BIN_DIR)%,$(TEST_OUTS))


        # List of source files
LIB_SOURCES=$(shell find $(SOURCE_DIR) -type f -name "*.cpp")
        # List of output files
LIB_OUTS =$(patsubst %.cpp,%.o,$(LIB_SOURCES))
LIB_OUTS:=$(patsubst $(SOURCE_DIR)%,$(LIB_DIR)%,$(LIB_OUTS))

default:
	@echo "Please make target 'libs' or 'test'"
	@echo "TODO: more complete makefile"
	@echo "Current line count: $(shell find ./** -not -path "./include/ebb/external/*" -type f \( -name "*.cpp" -o -name "*.hpp" \) -exec cat {} \; | wc -l)"

test: init libs $(TEST_OUTS)

init:
	rm -rf lib/
	rm -f bin/test_serialize
	rm -f bin/test_deserialize


$(TEST_OUTS): $(BIN_DIR)%: $(TEST_DIR)%.cpp
	@echo "Building test file $<"
	@mkdir -p $(dir $@)
	@$(CC) $(CCARGS) $(LDARGS) -I$(INCLUDE_PATH) -L$(BASE_PATH)$(LIB_DIR) -l$(basename $(patsubst lib%,%,$(LIB_FINAL))) $< -o $@
	@echo "Built $@"
	@echo "Running test $@"
	@gdb $@


libs: $(LIB_OUTS)
	@echo "Archiving libraries"
	@rm -f $(LIB_DIR)$(LIB_FINAL)
	@$(CC) -shared -o $(LIB_DIR)$(LIB_FINAL) $(LIB_OUTS) -fPIC
	@echo "Created final library file at '$(LIB_DIR)$(LIB_FINAL)'"

$(LIB_OUTS): $(LIB_DIR)%.o: $(SOURCE_DIR)%.cpp
	@echo "Building library file $<"
	@mkdir -p $(dir $@)
	@$(CC) $(CCARGS) $(COMPILE_ARGS) -I$(INCLUDE_PATH) $< -o $@ -fPIC
	@echo "Built $@"