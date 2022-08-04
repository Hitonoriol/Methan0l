include ../makeutils.inc

DEV=../build/include
LIB_PATH=../build

MODULES=$(patsubst ../modules/%/,%,$(dir $(call rwildcard,../modules,*.cpp)))

DEPS=-L$(LIB_PATH) -l:libmethan0l.so -Wl,--gc-sections
FLAGS=-O3 -fdata-sections -ffunction-sections

.PHONY: all clean

# Module's main source file must have the same name as its root directory
# Any additional .so and/or .a libraries must be located under `<module_name>/lib`
# 	Windows libraries must have a `-w` suffix, for example: `libfoo-w.a`
# Any additional headers must be located under `<module_name>/include`
define build_module
  @echo 'Building module $(1)...'
  $(eval -include ./$(1)/makefile.inc)
  $(eval SRCS:=$(call rwildcard,./$(1),*.cpp))
  $(eval LIB_SUFFIX:=$(if $(filter $(OS),Windows_NT),-w,))
  $(eval MODULE_DEPS:=$(call rwildcard,$(1)/lib,*$(LIB_SUFFIX).so *$(LIB_SUFFIX).a))
  $(eval MODULE_DEPS:=$(patsubst $(1)/lib/lib%.a,-l%,$(MODULE_DEPS)))
  $(eval MODULE_DEPS:=$(patsubst $(1)/lib/lib%.so,-l:$(1)/lib/lib%.so,$(MODULE_DEPS)))
  $(eval MODULE_INCLUDES:=$(patsubst %,-I%,$(1)/include))
  g++ -std=c++17 $(MOD_FLAGS) -fPIC $(FLAGS) $(SRCS) -I$(DEV) $(MODULE_INCLUDES) -I$(1) -shared $(FLAGS) $(DEPS) -L$(1)/lib $(MODULE_DEPS) -Wl,-Bstatic $(M_LINK_STATIC) -Wl,-Bdynamic $(M_LINK_DYNAMIC) -Wl,-rpath,'$$ORIGIN' -o $(1)/$(1).so || true
  @echo 'Finished building module $(1)'
  @echo ' '
endef

all:
	@echo 'Modules to build: [$(MODULES)]'
	$(foreach module,$(MODULES),$(call build_module,$(module)))

module-%: %
	$(call build_module,$<)

clean:
	@echo $(foreach module,$(patsubst ./%/,%,$(MODULES)),$(shell rm -fv $(module)/*.o $(module)/*.so))
