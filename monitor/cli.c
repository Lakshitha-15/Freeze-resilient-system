#define _GNU_SOURCE
#include <ncurses.h>
#include <unistd.h>
#include "../../common/log.h"
#include "../monitor/stats.h"

void cli_start() {
    initscr();
    noecho();
    curs_set(0);
    timeout(1000);
    
    while (1) {
        clear();
        mvprintw(0, 0, "Freeze-Resilient Container Monitor - Press q to quit");
        mvprintw(2, 0, "ID           MEM     CPU     Status");
        mvprintw(3, 0, "-----------------------------------");
        
        // TODO: Read real stats
        ContainerStats stats = {0};
        read_container_stats(&stats, "cont1");
        mvprintw(4, 0, "cont1       %7ldK %3d%%  running", 
                stats.mem_usage/1024, stats.cpu_usage);
        
        refresh();
        
        int ch = getch();
        if (ch == 'q') break;
    }
    
    endwin();
}
