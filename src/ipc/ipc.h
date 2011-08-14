#ifndef _ipc_ipc_h_
#define _ipc_ipc_h_

#include <stddef.h>
#include <unistd.h>
#include <sys/types.h>

#define IPC_MAX_PACKET_LEN 512
#define IPC_MAX_CONNS 64

typedef struct ipc_t* ipc_t;

int ipc_init(void);

void ipc_end(void);

ipc_t ipc_create(void);

ipc_t ipc_establish(ipc_t conn, pid_t cpid);

void ipc_close(ipc_t conn);

int ipc_read(ipc_t conn, void* buff, size_t len);

int ipc_write(ipc_t conn, const void* buff, size_t len);

#endif
