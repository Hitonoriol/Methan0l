.SECONDEXPANSION: # Enable double expansion

include ../makeutils.inc

MODULES=$(patsubst ../modules/%/,%,$(dir $(call rwildcard,../modules,*.cpp)))

all: $(MODULES)
	@echo 'Done building modules: [$(MODULES)]'
	@echo ' '

$(MODULES):
	cd $@ && $(MAKE) && echo "Done building $@" || echo "Failed to build $@. Skipping..."
	@echo ' '

clean:
	@echo $(foreach module,$(patsubst ./%/,%,$(MODULES)),$(shell rm -fv $(module)/*.o $(module)/*.so))

.PHONY: all $(MODULES) clean