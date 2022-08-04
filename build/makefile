RM := rm -rf
-include ../makeutils.inc

BITS=64

ifeq ($(OS),Windows_NT) 
    OS_NAME:=Windows
else
    OS_NAME:=$(shell sh -c 'uname -s 2>/dev/null || echo Unknown')
endif

SRCS = $(call rwildcard,../src,*.cpp)
DEPS = $(patsubst ../src/%.cpp,./obj/%.d,$(SRCS))

CLI_SRCS = $(call rwildcard,../src/cli,*.cpp)
CLI_OBJS = $(patsubst ../src/%,./obj/%,$(patsubst %.cpp,%.o,$(CLI_SRCS)))
LIB_OBJS = $(patsubst ../src/%,./obj/%,$(patsubst %.cpp,%.o.pic,$(filter-out $(CLI_SRCS),$(SRCS))))
RPATH = -Wl,-rpath,'$$ORIGIN'
LIB_INCLUDE = include
LIB_HEADERS = $(patsubst ../src/%,./$(LIB_INCLUDE)/%,$(call rwildcard,../src,*.h))
LIB_INC = -I../src

CLI_BIN_NAME = $(addsuffix $(if $(filter $(OS_NAME),Windows),.exe,),methan0l)
LIB_NAME = libmethan0l.so

LINK_STATIC := -lboost_filesystem -lboost_system
LINK_DYNAMIC :=

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

.PHONY: all release modules clean lib-clean

all: $(CLI_BIN_NAME)

# Full release tarball: cli binary, interpreter library, headers and modules
release: all lib-headers modules
	@echo 'Creating release tarball...'
	tar -czf methan0l-$(OS_NAME)$(BITS).tar.gz $(CLI_BIN_NAME) $(LIB_INCLUDE) $(LIB_NAME) ../modules ../baker
	@echo 'Done!'

modules: lib lib-headers
	@echo 'Building modules...'
	cd ../modules; make clean; make || true
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
	g++ $(MACROS) -std=c++17 -O3 $(LIB_INC) -c -m$(BITS) -fPIC -MMD -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $@'
	@echo ' '

lib-headers: $(LIB_INCLUDE) $(LIB_HEADERS)
	@echo 'Finished copying headers'
	@echo ' '

$(LIB_INCLUDE)/%.h: ../src/%.h
	@mkdir -p $(dir $@)
	cp -rf $< $@

$(LIB_INCLUDE):
	mkdir -p $(LIB_INCLUDE)
	@echo 'Created header directory: $@'

lib-clean:
	$(RM) $(LIB_INCLUDE)/* $(LIB_OBJS) $(CLI_OBJS)

# cli targets
cli: $(CLI_BIN_NAME)

$(CLI_BIN_NAME): lib $(CLI_OBJS)
	@echo 'Building interpreter cli: $@'
	g++ -o $(CLI_BIN_NAME) $(CLI_OBJS) -m$(BITS) -Wl,-Bstatic $(LINK_STATIC) -Wl,-Bdynamic -L./ -l:$(LIB_NAME) $(LINK_DYNAMIC) $(RPATH)
	@echo 'Finished building interpreter cli: $@'
	@echo ' '

-include $(DEPS)

obj/%.o: ../src/%.cpp
	@echo 'Building target: $@, file: $<'
	@mkdir -p $(dir $@)
	g++ $(MACROS) -std=c++17 -O3 $(LIB_INC) -c -Wall -fmessage-length=0 -m$(BITS) -MMD -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $@'
	@echo ' '

clean: lib-clean
	-$(RM) $(LIB_NAME) $(CLI_BIN_NAME) $(DEPS)
	-@echo ' '