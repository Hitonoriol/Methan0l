#include <methan0l.h>
#include <CoreLibrary.h>

#ifdef __linux__
	#include <ncurses.h>
#else // Windows
	// MSYS2 only has static ncurses binaries as of Nov 2023
	#define NCURSES_STATIC
	#include <ncurses/ncurses.h>
#endif

LOAD_MODULE
{
	VAR(NULL) = (void*)0;

	/* ncurses state variable getters */
	function("stdscr", [&]() {return stdscr;});
	function("curscr", [&]() {return curscr;});
	function("color_pair", [&](short n) {return COLOR_PAIR(n);});

	/* ncurses constants */
	/* Attributes */
	REG_VAR(A_NORMAL)
    REG_VAR(A_STANDOUT)
    REG_VAR(A_UNDERLINE)
    REG_VAR(A_REVERSE)
    REG_VAR(A_BLINK)
    REG_VAR(A_DIM)
    REG_VAR(A_BOLD)
    REG_VAR(A_PROTECT)
    REG_VAR(A_INVIS)
    REG_VAR(A_ALTCHARSET)
    REG_VAR(A_CHARTEXT)
    
    /* Colors */
    REG_VAR(COLOR_BLACK)
    REG_VAR(COLOR_RED)
	REG_VAR(COLOR_GREEN)
    REG_VAR(COLOR_YELLOW)
    REG_VAR(COLOR_BLUE)
    REG_VAR(COLOR_MAGENTA)
    REG_VAR(COLOR_CYAN)
    REG_VAR(COLOR_WHITE)

	/* ncurses functions */
	REG_FUNC(attron)
	REG_FUNC(wattron)
	REG_FUNC(attrset)
	REG_FUNC(wattrset)
	REG_FUNC(attr_get)
	REG_FUNC(wattr_get)
	REG_FUNC(attroff)
	REG_FUNC(wattroff)
	REG_FUNC(chgat)
	REG_FUNC(mvchgat)
	
	REG_FUNC(start_color)
	REG_FUNC(init_pair)
	REG_FUNC(use_default_colors)
	
	REG_FUNC(initscr)
	REG_FUNC(endwin)
	REG_FUNC(clear)
	REG_FUNC(wclear)
	REG_FUNC(refresh)
	REG_FUNC(wrefresh)
	REG_FUNC(cbreak)

	function("getmaxyx", [](WINDOW *scr, Value &y, Value &x) {getmaxyx(scr, y, x);});
	REG_FUNC(getmaxx)
	REG_FUNC(getmaxy)
	REG_FUNC(getcurx)
	REG_FUNC(getcury)

	REG_FUNC(printw)
	REG_FUNC(mvprintw)
	REG_FUNC(wprintw)
	REG_FUNC(mvwprintw)

	REG_FUNC(move)
	REG_FUNC(mvaddch)
	REG_FUNC(addch)

	REG_FUNC(getch)
	REG_FUNC(getstr)

	REG_FUNC(raw)
	REG_FUNC(noecho)	
}