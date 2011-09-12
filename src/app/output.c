#include "app/output.h"
#include "communication/types.h"
#include <ncurses.h>

#define AIRLINE_INITIAL_LINE 6
#define TURN_LINE 2
#define MAP_LINE_1 3
#define MAP_LINE_2 4
#define INITIAL_COLUMN 1
#define FINAL_LINE_1 2
#define FINAL_LINE_2 3

void run_output(struct MessageQueue* outputMsgQueue) {
    //REMINDER: mvprintw(row,col,format,...)
    initscr();
    int turn = 0;
    struct Message msg;
    int curLine;
    int maxLine = AIRLINE_INITIAL_LINE;

    while ((msg = message_queue_pop(outputMsgQueue)).type != MessageTypeEndOutput) {
        if (msg.type == MessageTypeMapStatus) {
            erase();
            //Print number of turn
            mvprintw(TURN_LINE, INITIAL_COLUMN,"Status in turn number %d.", turn);
            turn++;
            //Print map status
            mvprintw(MAP_LINE_1, INITIAL_COLUMN, "%.2f%% of the needs in the map were already taken care of.", 
                    msg.payload.mapStatus.completionPercentage);
            mvprintw(MAP_LINE_2, INITIAL_COLUMN, "The needs in %d out of %d total cities were already taken care of.",
                    msg.payload.mapStatus.citiesSatisfied, msg.payload.mapStatus.totalCities);
            refresh();
            break; 
        } else if (msg.type == MessageTypeAirlineStatus) {
            //Print airline status
            curLine = AIRLINE_INITIAL_LINE + msg.payload.airlineStatus.id;
            if ( curLine > maxLine ) {
                maxLine = curLine;
            }
            mvprintw(curLine, INITIAL_COLUMN, "Airline number %d still has %d of its %d with some stock left.",
                    msg.payload.airlineStatus.id, msg.payload.airlineStatus.planesFlying, 
                    msg.payload.airlineStatus.totalPlanes);
            refresh();
            break;
        }
    }
    
    mvprintw(maxLine + FINAL_LINE_1, INITIAL_COLUMN, 
            "The simulation is over! The world is now safe from various diseases!");
    mvprintw(maxLine + FINAL_LINE_2, INITIAL_COLUMN, 
            "You must truly be a simulation ninja wizard :D"); 
    refresh();
    endwin();    
    return;
}
