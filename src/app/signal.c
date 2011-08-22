#include "app/signal.h"

#include <signal.h>
#include <stddef.h>
#include <pthread.h>

#include "util.h"

#define HANDLE(sig) signal(sig, signal_handler)
#define IGNORE(sig) signal(sig, signal_ignore)

static void signal_handler(int sig);

static void signal_ignore(int sig);

static void (*exitFunction)(void) = NULL;

void register_exit_function(void (*onExit)(void)) {
    exitFunction = onExit;
}

void register_signal_handlers(void) {
    HANDLE(SIGHUP);
    HANDLE(SIGINT);
    HANDLE(SIGQUIT);
    HANDLE(SIGILL);
    HANDLE(SIGTRAP);
    HANDLE(SIGABRT);
    HANDLE(SIGEMT);
    HANDLE(SIGFPE);
    HANDLE(SIGBUS);
    HANDLE(SIGSEGV);
    HANDLE(SIGSYS);
    HANDLE(SIGPIPE);
    HANDLE(SIGALRM);
    HANDLE(SIGTERM);
    IGNORE(SIGTSTP);
    HANDLE(SIGCHLD);
}

void signal_handler(int sig) {
    // Things just go boom
#ifdef DEBUG
    switch (sig) {
        case SIGHUP:
            mprintf("Caught signal SIGHUP\n");
            break;
        case SIGINT:
            mprintf("Caught signal SIGINT\n");
            break;
        case SIGQUIT:
            mprintf("Caught signal SIGQUIT\n");
            break;
        case SIGILL:
            mprintf("Caught signal SIGILL\n");
            break;
        case SIGTRAP:
            mprintf("Caught signal SIGTRAP\n");
            break;
        case SIGABRT:
            mprintf("Caught signal SIGABRT\n");
            break;
        case SIGEMT:
            mprintf("Caught signal SIGEMT\n");
            break;
        case SIGFPE:
            mprintf("Caught signal SIGFPE\n");
            break;
        case SIGBUS:
            mprintf("Caught signal SIGBUS\n");
            break;
        case SIGSEGV:
            mprintf("Caught signal SIGSEGV\n");
            break;
        case SIGSYS:
            mprintf("Caught signal SIGSYS\n");
            break;
        case SIGPIPE:
            mprintf("Caught signal SIGPIPE\n");
            break;
        case SIGALRM:
            mprintf("Caught signal SIGALRM\n");
            break;
        case SIGTERM:
            mprintf("Caught signal SIGTERM\n");
            break;
        case SIGCHLD:
            mprintf("Caught signal SIGCHLD\n");
            break;
        default:
            mprintf("Caught signal %d\n", sig);
            break;
    }
#ifdef VERBOSE
    print_trace();
#endif
#endif
    if (exitFunction) {
        exitFunction();
    }
    exit(1);
}

void signal_ignore(int sig) {
    // What? I didn't catch that, can you repeat it?
}

