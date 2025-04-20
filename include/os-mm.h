#ifndef OSMM_H
#define OSMM_H


#define MM_PAGING
#define PAGING_MAX_MMSWP 4 /* max number of supported swapped space */ // Tối đa 4 vùng swap
#define PAGING_MAX_SYMTBL_SZ 30 // Tối đa 30 biến cho mỗi process

typedef char BYTE;
typedef uint32_t addr_t;
//typedef unsigned int uint32_t;

struct pgn_t{
   int pgn;
   struct pgn_t *pg_next; 
};

/*
 *  Memory region struct
 */
struct vm_rg_struct {
   unsigned long rg_start;
   unsigned long rg_end;

   struct vm_rg_struct *rg_next;
};

/*
 *  Memory area struct
 */
struct vm_area_struct {
   unsigned long vm_id;
   unsigned long vm_start;
   unsigned long vm_end;

   unsigned long sbrk;
/*
 * Derived field
 * unsigned long vm_limit = vm_end - vm_start
 */
   struct mm_struct *vm_mm; // trở ngược về mm_struct (vùng nhớ tổng thể của process)
   struct vm_rg_struct *vm_freerg_list; // Danh sách rg trống trong area
   struct vm_area_struct *vm_next; // Trỏ đến area tiếp theo
};

/* 
 * Memory management struct
 */
struct mm_struct {
   uint32_t *pgd; // Con trỏ đến page global directory - bảng trang cấp 1

   struct vm_area_struct *mmap; // Danh sách area 

   /* Currently we support a fixed number of symbol */
   struct vm_rg_struct symrgtbl[PAGING_MAX_SYMTBL_SZ];

   /* list of free page */
   struct pgn_t *fifo_pgn; // Danh sách các trang đã cấp phát
};

/*
 * FRAME/MEM PHY struct - Mô phỏng frames vật lý
 */
struct framephy_struct { 
   int fpn; // Số thứ tự frame vật lý
   struct framephy_struct *fp_next;

   /* Resereed for tracking allocated framed */
   struct mm_struct* owner; // process đang giữ frame này
};

struct memphy_struct {
   /* Basic field of data and size */
   BYTE *storage;
   int maxsz; // Kích thước tối đa của vùng nhớ vật lý
   
   /* Sequential device fields */ 
   int rdmflg; // Truy cập ngẫu nhiên/tuần tự?
   int cursor; // Vị trí con trỏ nếu là tuần tự

   /* Management structure */
   struct framephy_struct *free_fp_list; // Danh sách frames trống
   struct framephy_struct *used_fp_list; // Danh sách frames dùng
};

#endif
