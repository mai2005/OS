#ifndef MEM_H
#define MEM_H

#include "common.h"

#define RAM_SIZE	(1 << ADDRESS_SIZE) // RAM size = 2^20 = 1MB

/* Init related parameters, must be called before being used */
void init_mem(void);

/* Allocate [size] bytes for process [proc] and return its virtual address.
 * If we cannot allocate new memory region for this process, return 0 */
/* Cấp phát [size] bute cho tiến trình [proc] và trả về địa chỉ ảo nếu cấp phát thành công
   trả về 0 nếu thất bại */
addr_t alloc_mem(uint32_t size, struct pcb_t * proc);

/* Free a memory block having the first byte at [address] used by
 * process [proc]. Return 0 if [address] is valid. Otherwise, return 1 */
/* Giải phóng vùng nhớ bắt đầu từ address
   trả về 0 nếu thành công, 1 nếu lỗi*/
int free_mem(addr_t address, struct pcb_t * proc);

/* Read 1 byte memory pointed by [address] used by process [proc] and
 * save it to [data].
 * If the given [address] is valid, return 0. Otherwise, return 1 */
int read_mem(addr_t address, struct pcb_t * proc, BYTE * data);

/* Write [data] to 1 byte on the memory pointed by [address] of process
 * [proc]. If given [address] is valid, return 0. Otherwise, return 1 */
int write_mem(addr_t address, struct pcb_t * proc, BYTE data);

void dump(void);

#endif


