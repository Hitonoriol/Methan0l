.SECONDARY: # Disable intermediate file deletion
.SECONDEXPANSION: # Enable double expansion

-include ../makeutils.inc

RM=rm -rf

ifeq ($(OS),Windows_NT) 
    OS_NAME:=Windows
else
    OS_NAME:=$(shell sh -c 'uname -s 2>/dev/null || echo Unknown')
endif

# Configurable parameters
BITS=64
CONFIGURATION=release

# Build output directories
OUT_DIR = $(CONFIGURATION)/
OBJ_DIR = $(OUT_DIR)obj/
INCLUDE_DIR=$(OUT_DIR)include
STDLIB_DIR=$(OUT_DIR)libraries
MODULE_DIR=$(OUT_DIR)modules

# Global inputs
SRC_DIR = ../src/
SRCS = $(call rwildcard,$(SRC_DIR),*.cpp)
DEPS = $(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.d,$(SRCS))
EXAMPLES_DIR = ../examples/

# Stdlib inputs / outputs
STDLIB_PATH = stdlib/
STDLIB_SCRIPTS = $(patsubst $(SRC_DIR)$(STDLIB_PATH)%,$(STDLIB_DIR)/%,$(call rwildcard,$(SRC_DIR)$(STDLIB_PATH),*.mt0))
STDLIB_SRCS = $(call rwildcard,$(SRC_DIR)$(STDLIB_PATH),*.cpp)
STDLIB_OBJS = $(patsubst $(SRC_DIR)%,$(OBJ_DIR)%,$(patsubst %.cpp,%.o.pic,$(STDLIB_SRCS)))
STDLIB_BINS = $(patsubst $(SRC_DIR)$(STDLIB_PATH)%.cpp,$(STDLIB_DIR)/%.so,$(wildcard $(SRC_DIR)$(STDLIB_PATH)*.cpp))
SRCS:=$(filter-out $(STDLIB_SRCS),$(SRCS))

# CLI inputs / outputs
CLI_SRCS = $(call rwildcard,$(SRC_DIR)cli,*.cpp)
CLI_OBJS = $(patsubst $(SRC_DIR)%,$(OBJ_DIR)%,$(patsubst %.cpp,%.o,$(CLI_SRCS)))
LIB_OBJS = $(patsubst $(SRC_DIR)%,$(OBJ_DIR)%,$(patsubst %.cpp,%.o.pic,$(filter-out $(CLI_SRCS),$(SRCS))))
RPATH = -Wl,-rpath,'$$ORIGIN'
LIB_HEADERS = $(patsubst $(SRC_DIR)%,$(INCLUDE_DIR)/%,$(call rwildcard,$(SRC_DIR),*.h))
LIB_INC = -I$(SRC_DIR)

# Binary outputs
CLI_BIN_NAME = $(OUT_DIR)$(addsuffix $(if $(filter $(OS_NAME),Windows),.exe,),methan0l)
LIB_NAME = $(OUT_DIR)libmethan0l.so

# Static / dynamic library dependencies
LINK_STATIC := -lboost_filesystem-mt -lboost_system-mt
LINK_DYNAMIC :=

# GCC compilation flags
FLAGS:=-std=c++2a -Wall -Wextra -Wno-unused-parameter -Wno-overloaded-virtual
ifeq ($(CONFIGURATION),debug)
	FLAGS += -Og -g
else
	FLAGS += -O3
endif

# Macros to pass to compiler
MACROS = -DBITS=$(BITS)

# Pass NO_STATIC variable to make to link everything dynamically
ifdef NO_STATIC
	LINK_DYNAMIC+= $(LINK_STATIC)
	LINK_STATIC=
endif

ifneq ($(OS_NAME),Windows)
	ifeq ($(OS_NAME),Linux)
		LINK_DYNAMIC+= -ldl
	endif
endif

.PHONY: all lib lib-headers stdlib install release modules clean lib-clean

all: $(CLI_BIN_NAME)

# Installation target (Linux only)
INSTALL_DIR=/opt/methan0l
install: all lib-headers
	@if [ "$(OS_NAME)" = "Linux" ]; then \
	    echo "Installing $(CLI_BIN_NAME)..." && \
	    mkdir -p $(INSTALL_DIR) && \
	    cp -f $(LIB_NAME) $(INSTALL_DIR) && \
	    cp -f $(CLI_BIN_NAME) $(INSTALL_DIR) && \
	    cp -rf $(MODULE_DIR) $(INSTALL_DIR) && \
	    cp -rf $(STDLIB_DIR) $(INSTALL_DIR) && \
	    cp -rf $(INCLUDE_DIR) $(INSTALL_DIR) && \
	    cp -rf $(EXAMPLES_DIR) $(INSTALL_DIR) && \
	    ln -sf $(INSTALL_DIR)/$(CLI_BIN_NAME) /usr/local/bin/$(CLI_BIN_NAME) && \
	    ln -sf $(INSTALL_DIR)/$(LIB_NAME) /usr/local/lib/$(LIB_NAME) && \
	    ln -sf $(INSTALL_DIR)/$(INCLUDE_DIR) /usr/local/include/methan0l && \
	    echo "export METHAN0L_HOME=$(INSTALL_DIR)" > /etc/profile.d/methan0l.sh && \
	    echo "Installation finished successfully" || echo "Installation failed"; \
	fi

-include $(DEPS)

# Full release tarball: cli binary, interpreter library, headers and modules
release: all lib-headers modules
	@echo 'Creating release tarball...'
	tar -czf methan0l-$(OS_NAME)$(BITS).tar.gz $(CLI_BIN_NAME) $(INCLUDE_DIR) $(LIB_NAME) $(MODULE_DIR) $(STDLIB_DIR) ../baker $(EXAMPLES_DIR)
	@echo 'Done!'

# (Attempt to) Build all modules
modules: lib lib-headers
	@echo 'Building modules...'
	cd ../modules && make
	@echo 'Finished building modules'
	@echo ' '

# ---------- Interpreter shared library targets ----------
lib: $(LIB_NAME)

$(LIB_NAME): $(LIB_OBJS)
	@echo 'Building interpreter library: $@...'
	g++ $(FLAGS) $(LIB_OBJS) -shared -o $(LIB_NAME) -Wl,-Bstatic $(LINK_STATIC) -Wl,-Bdynamic $(LINK_DYNAMIC) $(RPATH)
	@echo 'Finished building: $@'
	@echo ' '

$(OBJ_DIR)%.o.pic: $(SRC_DIR)%.cpp
	@echo 'Building target: $@ (file: $<)'
	@mkdir -p $(dir $@)
	g++ $(FLAGS) $(MACROS) $(LIB_INC) -c -m$(BITS) -fPIC -MMD -MP -MF"$(@:%.o.pic=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $@'
	@echo ' '

lib-headers: $(INCLUDE_DIR) $(LIB_HEADERS)
	@echo 'Finished copying headers'
	@echo ' '

$(INCLUDE_DIR)/%.h: $(SRC_DIR)%.h
	@mkdir -p $(dir $@)
	cp -rf $< $@

$(INCLUDE_DIR):
	mkdir -p $(INCLUDE_DIR)
	mkdir -p $(MODULE_DIR)

lib-clean:
	-$(RM) $(INCLUDE_DIR) $(LIB_OBJS) $(STDLIB_OBJS)
# --------------------------------------------------------

# ------------ Methan0l core library targets -------------
read_stdlib_deps=$(patsubst %,$(OBJ_DIR)$(STDLIB_PATH)%.o.pic,$(file < $(SRC_DIR)$(STDLIB_PATH)$(1).deps))

$(STDLIB_DIR):
	mkdir -p $(STDLIB_DIR)

stdlib: $(STDLIB_DIR) lib lib-headers $(STDLIB_BINS) $(STDLIB_SCRIPTS)
	@echo 'Finished building methan0l core libraries'
	@echo ' '

$(STDLIB_DIR)/%.so: $(OBJ_DIR)$$(STDLIB_PATH)%.o.pic lib $$(call read_stdlib_deps,%)
	@echo 'Building library: $@'
	$(eval lib_obj:=$(word 1,$^))
	$(eval lib_dep:=$(file < $(patsubst $(OBJ_DIR)%.o.pic,$(SRC_DIR)%.deps,$(lib_obj))))
	$(eval lib_dep:=$(patsubst %,$(OBJ_DIR)$(STDLIB_PATH)%.o.pic,$(lib_dep)))
	g++ $(lib_obj) $(lib_dep) -shared -L./ -l:$(LIB_NAME) -o $@ $(RPATH)
	@echo 'Finished building core library: $@'
	@echo ' '

$(STDLIB_DIR)/%.mt0: $(SRC_DIR)$(STDLIB_PATH)%.mt0
	@mkdir -p $(dir $@)
	cp -rf $< $@
# --------------------------------------------------------

# ---------------------- CLI targets ---------------------
cli: $(CLI_BIN_NAME)

$(CLI_BIN_NAME): lib stdlib $(CLI_OBJS)
	@echo 'Building interpreter cli: $@'
	g++ $(FLAGS) -o $(CLI_BIN_NAME) $(CLI_OBJS) -m$(BITS) -Wl,-Bstatic $(LINK_STATIC) -Wl,-Bdynamic -L./ -l:$(LIB_NAME) $(LINK_DYNAMIC) $(RPATH)
	@echo 'Finished building interpreter cli: $@'
	@echo ' '

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@echo 'Building target: $@, file: $<'
	@mkdir -p $(dir $@)
	g++ $(FLAGS) $(MACROS) $(LIB_INC) -c -Wall -fmessage-length=0 -m$(BITS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $@'
	@echo ' '
# --------------------------------------------------------

clean: lib-clean
	-$(RM) $(CLI_BIN_NAME) $(LIB_NAME) $(DEPS) $(CLI_OBJS) $(STDLIB_DIR)/* $(OBJ_DIR)/*
	-@echo ' '