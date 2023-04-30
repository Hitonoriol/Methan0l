rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRCS:=$(call rwildcard,.,*.cpp)
OBJS:=$(patsubst %.cpp,%.o,$(SRCS))

DEV_HEADERS=$(LIB_PATH)/include
OUT_PATH=$(LIB_PATH)/modules

DEPS=-L$(LIB_PATH) -l:libmethan0l.so -Wl,--gc-sections
FLAGS=-std=c++17 -O3 -fdata-sections -ffunction-sections

MODULE_BIN:=$(MODULE_NAME).so
MODULE_DIR:=$(OUT_PATH)/$(MODULE_NAME)

MODULE_INCLUDES:=-I./ -I./include

ifeq ($(OS),Windows_NT)
	M_LINK_STATIC+=$(M_LINK_DYNAMIC)
	M_LINK_DYNAMIC= 
endif

all: $(MODULE_BIN)

$(MODULE_DIR):
	mkdir -p $@

$(MODULE_BIN): $(MODULE_DIR) $(OBJS)
	g++ $(FLAGS) $(MOD_FLAGS) -shared $(DEPS) $(OBJS) -Wl,-Bstatic $(M_LINK_STATIC) -Wl,-Bdynamic $(M_LINK_DYNAMIC) -Wl,-rpath,'$$ORIGIN' -o $(MODULE_DIR)/$(MODULE_BIN)
	@echo ' '

%.o: %.cpp
	g++ -c $(FLAGS) $(MOD_FLAGS) -I$(DEV_HEADERS) $(MODULE_INCLUDES) -fPIC $< -o $@
	@echo ' '

clean:
	rm -fv $(OBJS) $(MODULE_BIN)