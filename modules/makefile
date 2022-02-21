include ../makeutils.inc

DEV=../build/methan0l-sdk
LIB_PATH=../build

MODULES=$(patsubst ../modules/%,./%,$(sort $(dir $(wildcard ../modules/*/.))))

DEPS=-L$(LIB_PATH) -l:libmethan0l.so -Wl,--gc-sections
FLAGS=-O3 -fdata-sections -ffunction-sections

.PHONY: all clean make
all:
	@echo $(MODULES)
	$(foreach module,$(patsubst ./%/,%,$(MODULES)),$(MAKE) module-$(module))

# Module's main source file must have the same name as its root directory
# Any additional .so and/or .a libraries must be located under `<module_name>/lib`
# 	Windows libraries must have a `-w` suffix, for example: `libfoo-w.a`
# Any additional headers must be located under `<module_name>/include`
module-%: ./%
	$(eval dir:=$(@:%/=%))
	$(eval dir:=$(dir:module-%=%))
	@echo Building module $(dir)...
	
	$(eval -include $(dir)/makefile.inc)
	$(eval SRCS:=$(call rwildcard,./$(dir),*.cpp))
	$(eval LIB_SUFFIX:=$(if $(filter $(OS),Windows_NT),-w,))
	$(eval MODULE_DEPS:=$(call rwildcard,$(dir)/lib,*$(LIB_SUFFIX).so *$(LIB_SUFFIX).a))
	$(eval MODULE_DEPS:=$(patsubst $(dir)/lib/lib%.a,-l%,$(MODULE_DEPS)))
	$(eval MODULE_DEPS:=$(patsubst $(dir)/lib/lib%.so,-l:$(dir)/lib/lib%.so,$(MODULE_DEPS)))
	$(eval MODULE_INCLUDES:=$(patsubst %,-I%,$(dir)/include))
	g++ -std=c++17 $(MOD_FLAGS) -fPIC -no-pie $(FLAGS) $(SRCS) -I$(DEV) $(MODULE_INCLUDES) -I$(dir) -shared $(FLAGS) $(DEPS) -L$(dir)/lib $(MODULE_DEPS) -Wl,-Bstatic $(LINK_STATIC) -Wl,-Bdynamic $(LINK_DYNAMIC) -o $(dir)/$(dir).so || true

make:
	@echo ''

clean:
	@echo $(foreach module,$(patsubst ./%/,%,$(MODULES)),$(shell rm -fv $(module)/*.o $(module)/*.so))
