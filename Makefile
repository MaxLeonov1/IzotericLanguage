CXX := g++

SRC_DIR := src
FRONTEND_DIR  := $(SRC_DIR)/frontend
MIDDLEEND_DIR := $(SRC_DIR)/middleend
BACKEND_DIR   := $(SRC_DIR)/backend
COMMON_DIR    := $(SRC_DIR)/common
TREE_DIR      := $(SRC_DIR)/tree
STACK_DIR     := $(SRC_DIR)/stack 

INCLUDES := -I $(FRONTEND_DIR) -I $(MIDDLEEND_DIR) -I $(BACKEND_DIR) \
            -I $(COMMON_DIR) -I $(TREE_DIR) -I $(STACK_DIR)

FRONTEND_SOURCES  := $(wildcard $(FRONTEND_DIR)/*.cpp)
MIDDLEEND_SOURCES := $(wildcard $(MIDDLEEND_DIR)/*.cpp)
BACKEND_SOURCES   := $(wildcard $(BACKEND_DIR)/*.cpp)
COMMON_SOURCES    := $(wildcard $(COMMON_DIR)/*.cpp)
TREE_SOURCES      := $(wildcard $(TREE_DIR)/*.cpp)
STACK_SOURCES     := $(wildcard $(STACK_DIR)/*.cpp)

OBJ_DIR := obj
FRONTEND_OBJECTS  := $(FRONTEND_SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
MIDDLEEND_OBJECTS := $(MIDDLEEND_SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
BACKEND_OBJECTS   := $(BACKEND_SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
COMMON_OBJECTS    := $(COMMON_SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TREE_OBJECTS      := $(TREE_SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
STACK_OBJECTS     := obj/stack/stack.o         #WTF

ALL_OBJECTS := $(FRONTEND_OBJECTS) $(MIDDLEEND_OBJECTS) $(BACKEND_OBJECTS) \
               $(COMMON_OBJECTS) $(TREE_OBJECTS) $(STACK_OBJECTS)

FRONTEND_TARGET  := frontend.exe
MIDDLEEND_TARGET := middleend.exe
BACKEND_TARGET   := backend.exe

DEBUG_FLAGS := -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations \
               -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts \
               -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal \
               -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op \
               -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self \
               -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel \
               -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types \
               -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code \
               -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers \
               -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new \
               -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging \
               -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla

RELEASE_FLAGS := -O2 -std=c++17

SANITIZER_LDFLAGS := -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr 

all: debug

debug: CXXFLAGS = $(DEBUG_FLAGS) $(INCLUDES)
debug: LDFLAGS = $(SANITIZER_LDFLAGS)
debug: $(FRONTEND_TARGET) $(MIDDLEEND_TARGET) $(BACKEND_TARGET)

release: CXXFLAGS = $(RELEASE_FLAGS) $(INCLUDES)
release: LDFLAGS =
release: $(FRONTEND_TARGET) $(MIDDLEEND_TARGET) $(BACKEND_TARGET)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)/frontend
	@mkdir -p $(OBJ_DIR)/middleend
	@mkdir -p $(OBJ_DIR)/backend
	@mkdir -p $(OBJ_DIR)/common
	@mkdir -p $(OBJ_DIR)/tree
	@mkdir -p $(OBJ_DIR)/stack

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(FRONTEND_TARGET): $(FRONTEND_OBJECTS) $(COMMON_OBJECTS) $(TREE_OBJECTS) $(STACK_OBJECTS)
	$(CXX) $(FRONTEND_OBJECTS) $(COMMON_OBJECTS) $(TREE_OBJECTS) $(STACK_OBJECTS) -o $@ $(LDFLAGS)
	@chmod +x $@
	@echo "Frontend built"

$(MIDDLEEND_TARGET): $(MIDDLEEND_OBJECTS) $(COMMON_OBJECTS) $(TREE_OBJECTS) $(STACK_OBJECTS)
	$(CXX) $(MIDDLEEND_OBJECTS) $(COMMON_OBJECTS) $(TREE_OBJECTS) $(STACK_OBJECTS) -o $@ $(LDFLAGS)
	@chmod +x $@
	@echo "Middleend built"

$(BACKEND_TARGET): $(BACKEND_OBJECTS) $(COMMON_OBJECTS) $(TREE_OBJECTS) $(STACK_OBJECTS)
	$(CXX) $(BACKEND_OBJECTS) $(COMMON_OBJECTS) $(TREE_OBJECTS) $(STACK_OBJECTS) -o $@ $(LDFLAGS)
	@chmod +x $@
	@echo "Backend built"

clean: logclean
	@rm -rf $(OBJ_DIR)
	@rm -f $(FRONTEND_TARGET) $(MIDDLEEND_TARGET) $(BACKEND_TARGET)
	@echo "Cleaned all build files"

logclean:
	@rm -rf logs
	@rm -f tex_log.tex

frontend: CXXFLAGS = $(DEBUG_FLAGS) $(INCLUDES)
frontend: LDFLAGS = $(SANITIZER_LDFLAGS)
frontend: $(FRONTEND_TARGET)

middleend: CXXFLAGS = $(DEBUG_FLAGS) $(INCLUDES)
middleend: LDFLAGS = $(SANITIZER_LDFLAGS)
middleend: $(MIDDLEEND_TARGET)

backend: CXXFLAGS = $(DEBUG_FLAGS) $(INCLUDES)
backend: LDFLAGS = $(SANITIZER_LDFLAGS)
backend: $(BACKEND_TARGET)