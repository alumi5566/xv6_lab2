#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

struct {
    struct spinlock lock;
    struct shm_page {
        uint id;
        char *frame;
        int refcnt;
    } shm_pages[64];
} shm_table;

void shminit() {
    int i;
    initlock(&(shm_table.lock), "SHM lock");
    acquire(&(shm_table.lock));
    for (i = 0; i< 64; i++) {
        shm_table.shm_pages[i].id =0;
        shm_table.shm_pages[i].frame =0;
        shm_table.shm_pages[i].refcnt =0;
    }
    release(&(shm_table.lock));
}

int shm_open(int id, char **pointer) {
#if 1
    //you write this
	uint i;
	uint found = 0;
	//char *va;
	char *mem;
	for(i=0;i<64;i++){
		if(shm_table.shm_pages[i].id == id){
            //hit
            cprintf("HIT!!\n");
            found = 1;
            acquire(&(shm_table.lock));
		  	  shm_table.shm_pages[i].refcnt++;
            release(&(shm_table.lock));
            //mapping
            //char *va = PGROUNDUP(myproc()->sz);
            cprintf("HIT2!!\n");
            uint va1 = PGROUNDUP(myproc()->sz);
            if(mappages(myproc()->pgdir, (char*)va1, PGSIZE, V2P(shm_table.shm_pages[i].frame), PTE_W|PTE_U) < 0){
                cprintf("allocuvm out of memory (2)\n");
            }
            //return this va
            *pointer = (char *)va1;
            //update sz
            myproc()->sz = PGROUNDUP(myproc()->sz) + PGSIZE;
            cprintf("HIT3!!\n");
            //release(&(shm_table.lock));
            //break;
            goto end;
		}
	}
	if(!found){
		//get a unused element
		for(i=0;i<64;i++){
            if(shm_table.shm_pages[i].refcnt == 0){
                cprintf("NEW!!!\n");
                //inc refcnt, assign id
                acquire(&(shm_table.lock));
                  shm_table.shm_pages[i].refcnt++;
                  shm_table.shm_pages[i].id = id;
                release(&(shm_table.lock));
                //allocate a page and save to *frame
                mem = kalloc();
                cprintf("NEW2!!!\n");
                if(mem == 0){
                    cprintf("allocuvm out of memory\n");
                    //deallocuvm(pgdir, newsz, oldsz);
                    //return 0;
                }else{
                    memset(mem, 0, PGSIZE);
                    acquire(&(shm_table.lock));
                      shm_table.shm_pages[i].frame = mem;
                    release(&(shm_table.lock));
                }
                //have a segment in table, mappage() to it
                uint va2 = PGROUNDUP(myproc()->sz);
                if(mappages(myproc()->pgdir, (char*)va2, PGSIZE, V2P(shm_table.shm_pages[i].frame), PTE_W|PTE_U) < 0){
                    cprintf("allocuvm out of memory (2)\n");
                }
                cprintf("NEW3!!!\n");
                //return this va
                *pointer = (char *)va2;
                //update sz
                myproc()->sz = PGROUNDUP(myproc()->sz) + PGSIZE;
                //release(&(shm_table.lock));
                //break;
                goto end;
            }
            
		}
	}
end:
#endif
    return 0; //added to remove compiler warning -- you should decide what to return
}


int shm_close(int id) {
#if 1
    //you write this too!
	for(uint i=0;i<64;i++){
		if(shm_table.shm_pages[i].id == id){
            //hit
            acquire(&(shm_table.lock));
		  	shm_table.shm_pages[i].refcnt--;
            release(&(shm_table.lock));
            break;
		}
	}
#endif
    return 0; //added to remove compiler warning -- you should decide what to return
}
