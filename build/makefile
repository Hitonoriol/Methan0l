.SECONDARY: # Disable intermediate file deletion
.SECONDEXPANSION: # Enable double expansion

-include ../makeutils.inc

RM=rm -rf
BITS=64

ifeq ($(OS),Windows_NT) 
    OS_NAME:=Windows
else
    OS_NAME:=$(shell sh -c 'uname -s 2>/dev/null || echo Unknown')
endif

INCLUDE_DIR=include
STDLIB_DIR=libraries
MODULE_DIR=modules

SRCS = $(call rwildcard,../src,*.cpp)
DEPS = $(patsubst ../src/%.cpp,./obj/%.d,$(SRCS))

STDLIB_PATH = lang/core/library
STDLIB_SRCS = $(call rwildcard,../src/$(STDLIB_PATH),*.cpp)
STDLIB_OBJS = $(patsubst ../src/%,./obj/%,$(patsubst %.cpp,%.o.pic,$(STDLIB_SRCS)))
STDLIB_BINS = $(patsubst ../src/$(STDLIB_PATH)/%.cpp,./$(STDLIB_DIR)/%.so,$(wildcard ../src/$(STDLIB_PATH)/*.cpp))
SRCS:=$(filter-out $(STDLIB_SRCS),$(SRCS))

CLI_SRCS = $(call rwildcard,../src/cli,*.cpp)
CLI_OBJS = $(patsubst ../src/%,./obj/%,$(patsubst %.cpp,%.o,$(CLI_SRCS)))
LIB_OBJS = $(patsubst ../src/%,./obj/%,$(patsubst %.cpp,%.o.pic,$(filter-out $(CLI_SRCS),$(SRCS))))
RPATH = -Wl,-rpath,'$$ORIGIN'
LIB_HEADERS = $(patsubst ../src/%,./$(INCLUDE_DIR)/%,$(call rwildcard,../src,*.h))
LIB_INC = -I../src

CLI_BIN_NAME = $(addsuffix $(if $(filter $(OS_NAME),Windows),.exe,),methan0l)
LIB_NAME = libmethan0l.so

LINK_STATIC := -lboost_filesystem -lboost_system
LINK_DYNAMIC :=

FLAGS:=-std=c++17 -Wall -Wextra -Wno-unused-parameter -O3

INSTALL_DIR=/opt/methan0l

MACROS = -DBITS=$(BITS)

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

install: all lib-headers
	@if [ "$(OS_NAME)" = "Linux" ]; then \
	    echo "Installing $(CLI_BIN_NAME)..."; \
	    mkdir -p $(INSTALL_DIR); \
	    cp -f $(LIB_NAME) $(INSTALL_DIR) && \
	    cp -f $(CLI_BIN_NAME) $(INSTALL_DIR) && \
	    cp -rf modules $(INSTALL_DIR) && \
	    cp -rf $(MODULE_DIR) $(INSTALL_DIR) && \
	    cp -rf $(STDLIB_DIR) $(INSTALL_DIR) && \
	    cp -rf $(INCLUDE_DIR) $(INSTALL_DIR) && \
	    ln -sf $(INSTALL_DIR)/$(CLI_BIN_NAME) /usr/local/bin/$(CLI_BIN_NAME) && \
	    ln -sf $(INSTALL_DIR)/$(LIB_NAME) /usr/local/lib/$(LIB_NAME) && \
	    ln -sf $(INSTALL_DIR)/$(INCLUDE_DIR) /usr/local/include/methan0l && \
	    echo "Installation finished successfully" || echo "Installation failed"; \
	fi

-include $(DEPS)

# Full release tarball: cli binary, interpreter library, headers and modules
release: all lib-headers modules
	@echo 'Creating release tarball...'
	tar -czf methan0l-$(OS_NAME)$(BITS).tar.gz $(CLI_BIN_NAME) $(INCLUDE_DIR) $(LIB_NAME) modules $(STDLIB_DIR) ../baker
	@echo 'Done!'

modules: lib lib-headers
	@echo 'Building modules...'
	cd ../modules && make
	@echo 'Finished building modules'
	@echo ' '

# Interpreter shared library targets
lib: libmethan0l.so

libmethan0l.so: $(LIB_OBJS)
	@echo 'Building interpreter library: $@...'
	g++ $(LIB_OBJS) -shared -Wl,-Bstatic $(LINK_STATIC) -Wl,-Bdynamic $(LINK_DYNAMIC) -o $(LIB_NAME) $(RPATH)
	@echo 'Finished building: $@'
	@echo ' '

obj/%.o.pic: ../src/%.cpp
	@echo 'Building target: $@ (file: $<)'
	@mkdir -p $(dir $@)
	g++ $(FLAGS) $(MACROS) $(LIB_INC) -c -m$(BITS) -fPIC -MMD -MP -MF"$(@:%.o.pic=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $@'
	@echo ' '

lib-headers: $(INCLUDE_DIR) $(LIB_HEADERS)
	@echo 'Finished copying headers'
	@echo ' '

$(INCLUDE_DIR)/%.h: ../src/%.h
	@mkdir -p $(dir $@)
	cp -rf $< $@

$(INCLUDE_DIR):
	mkdir -p $(INCLUDE_DIR)
	mkdir -p $(MODULE_DIR)

lib-clean:
	-$(RM) $(INCLUDE_DIR) $(LIB_OBJS) $(STDLIB_OBJS)

# Methan0l core library targets
read_stdlib_deps=$(patsubst %,obj/$(STDLIB_PATH)/%.o.pic,$(file < ../src/$(STDLIB_PATH)/$(1).deps))

$(STDLIB_DIR):
	mkdir -p $(STDLIB_DIR)

stdlib: $(STDLIB_DIR) lib lib-headers $(STDLIB_BINS)
	@echo 'Finished building methan0l core libraries'
	@echo ' '

$(STDLIB_DIR)/%.so: obj/$$(STDLIB_PATH)/%.o.pic lib $$(call read_stdlib_deps,%)
	@echo 'Building library: $@'
	$(eval lib_obj:=$(word 1,$^))
	$(eval lib_dep:=$(file < $(patsubst obj/%.o.pic,../src/%.deps,$(lib_obj))))
	$(eval lib_dep:=$(patsubst %,./obj/$(STDLIB_PATH)/%.o.pic,$(lib_dep)))
	g++ $(lib_obj) $(lib_dep) -shared -L./ -l:$(LIB_NAME) -o $@ $(RPATH)
	@echo 'Finished building core library: $@'
	@echo ' '

# cli targets
cli: $(CLI_BIN_NAME)

$(CLI_BIN_NAME): lib stdlib $(CLI_OBJS)
	@echo 'Building interpreter cli: $@'
	g++ -o $(CLI_BIN_NAME) $(CLI_OBJS) -m$(BITS) -Wl,-Bstatic $(LINK_STATIC) -Wl,-Bdynamic -L./ -l:$(LIB_NAME) $(LINK_DYNAMIC) $(RPATH)
	@echo 'Finished building interpreter cli: $@'
	@echo ' '

obj/%.o: ../src/%.cpp
	@echo 'Building target: $@, file: $<'
	@mkdir -p $(dir $@)
	g++ $(FLAGS) $(MACROS) $(LIB_INC) -c -Wall -fmessage-length=0 -m$(BITS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $@'
	@echo ' '

clean: lib-clean
	-$(RM) $(CLI_BIN_NAME) $(LIB_NAME) $(DEPS) $(CLI_OBJS) $(STDLIB_DIR)/* obj/*
	-@echo ' '