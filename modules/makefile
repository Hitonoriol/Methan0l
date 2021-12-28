include ../makeutils.inc

DEV=../build/methan0l-dev
LIB_PATH=../build

MODULES=$(patsubst ../modules/%,./%,$(sort $(dir $(wildcard ../modules/*/.))))

DEPS=-L$(LIB_PATH) -Wl,-Bstatic -l:libmethan0l.so -Wl,-Bdynamic -Wl,--gc-sections
FLAGS=-O3 -fdata-sections -ffunction-sections

.PHONY: all
all: $(MODULES)

module-%: ./%

# Module's main source file must have the same name as its root directory
# Any additional .so and/or .a libraries must be located under `<module_name>/lib`
# 	Windows libraries must have a `-w` suffix, for example: `libfoo-w.a`
# Any additional headers must be located under `<module_name>/include`
./%:
	$(eval dir:=$(@:%/=%))
	@echo Building module $(dir)...
	
	$(eval SRCS:=$(call rwildcard,./$(dir),*.cpp))
	$(eval LIB_SUFFIX:=$(if $(filter $(OS),Windows_NT),-w,))
	$(eval MODULE_DEPS:=$(call rwildcard,$(dir)/lib,*$(LIB_SUFFIX).so *$(LIB_SUFFIX).a))
	$(eval MODULE_INCLUDES:=$(patsubst %,-I%,$(dir)/include))
	g++ -std=c++17 -fPIC $(FLAGS) $(SRCS) -I$(DEV)/include $(MODULE_INCLUDES) -I$(dir) -shared $(FLAGS) $(DEPS) $(addprefix -l:,$(MODULE_DEPS)) -o $(dir)/$(dir).so