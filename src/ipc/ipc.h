#ifndef _ipc_ipc_h_
#define _ipc_ipc_h_

#include <stddef.h>

#define IPC_MAX_PACKET_LEN 512

typedef struct ipc_t* ipc_t;

// It should be taken into account, by all bold users of this interface
// that calling ipc_listen before a fork call, will cause both process to respond
// to the same name.

int ipc_init(void);

int ipc_listen(const char* name);

void ipc_end(void);

ipc_t ipc_establish(const char* name);

void ipc_close(ipc_t conn);

int ipc_read(void* buff, size_t len);

int ipc_write(ipc_t conn, const void* buff, size_t len);

#endif
