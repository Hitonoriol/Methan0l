-include ../makefile.defs

DEV=../build/methan0l-dev
DEPS=-I$(DEV)/include -L$(DEV) -lmethan0l $(BOOST_DEPS) -Wl,--gc-sections
FLAGS=-O3 -fdata-sections -ffunction-sections

module-%:
	$(eval dir:=$(@:module-%=%))
	@echo Building module $(dir)...
	
	g++ -std=c++17 -fPIC $(FLAGS) -c $(dir)/$(dir).cpp -o $(dir)/$(dir).o $(DEPS)
	g++ -shared $(FLAGS) $(dir)/$(dir).o -o $(dir)/$(dir).so $(DEPS)