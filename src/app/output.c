#include "app/output.h"
#include "communication/types.h"
#include <ncurses.h>


void run_output(struct MessageQueue* outputMsgQueue) {

    //initscr();

    struct Message msg;
    while ((msg = message_queue_pop(outputMsgQueue)).type != MessageTypeEndOutput) {
        mprintf("ADENTRO DEL WHILE\n");
        if (msg.type == MessageTypeMapStatus) {
           break; 
        } else if (msg.type == MessageTypeAirlineStatus) {
           break;
        }
    }
    mprintf("fin del while\n"); 
    //TODO DECIR CHAU
    //endwin();    
    return;
}
