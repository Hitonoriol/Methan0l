LINK_STATIC:=
LINK_DYNAMIC:=-lncurses -lmenu -lform -lpanel
MOD_FLAGS:=

ifeq ($(OS),Windows_NT)
	LINK_STATIC=$(LINK_DYNAMIC)
	LINK_DYNAMIC= 
endif