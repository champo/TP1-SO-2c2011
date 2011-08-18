#ifndef _ipc_ipc_h_
#define _ipc_ipc_h_

#include <stddef.h>
#include <unistd.h>
#include <sys/types.h>

#define IPC_MAX_PACKET_LEN 512
#define IPC_MAX_CONNS 64

typedef struct ipc_t* ipc_t;

/**
 * A set of functions to use for IPC between a parent and several child processes.
 *
 * This is not meant to be used with more than one level of parent-child.
 * That is, if your parent is using this, it's quite likely everything will break
 * if you try to use this functions to comunicate with your child.
 */

int ipc_init(void);

void ipc_end(void);

ipc_t ipc_create(void);

ipc_t ipc_establish(ipc_t conn, pid_t cpid);

void ipc_close(ipc_t conn);

/**
 * Closes the connection to this process, without destroying it for any other that might be sharing it.
 */
void ipc_discard(ipc_t conn);

int ipc_read(ipc_t conn, void* buff, size_t len);

int ipc_write(ipc_t conn, const void* buff, size_t len);

/**
 * Check if any read operations are available.
 *
 * Warning: Using ipc_select and ipc_read on different threads at the same time
 * has undefined behaviour, and can result in a deadlock.
 *
 * @return the connection on which a read is ready.
 */
ipc_t ipc_select(void);

#endif
