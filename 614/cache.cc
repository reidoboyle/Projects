/**
 * Reid OBoyle
 * CSCE 614 
 * 9/27/2022
 * Homework2 
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cstring>
#include <string>
#include <math.h>
#include <list>

int BLOCK_SIZE;
int S;
struct Address
{
    int offset;
    int index;
    long long int tag;
};

struct Address process_address( long long int address)
{
    struct Address addr;
    addr.offset = address % BLOCK_SIZE;
    addr.index =    int(address >> int(log2(BLOCK_SIZE))) & (S-1);
    addr.tag = (address / BLOCK_SIZE) / S;
    return addr;
}
int random_replace(int assoc)
{
    srand((unsigned) time(0));
    return rand() % assoc;

}
void process_instructions(long long int* cache, bool random,int assoc)
{
    int ms = 0;     // total misses
    int rms = 0;    // read misses
    int wms = 0;    // write misses
    int ra = 0;     // read access total
    int wa = 0;     // write access total
    int ta = 0;     // total accesses
    long long int* moving_cache = cache;
    std::list<std::list<int>> LRU;
    if(!random)
    {
        for(int i=0;i<S;i++)
        {
            std::list<int> li;
            LRU.push_back(li);
        }
    }
    //struct Instruction ins;
    char type;
    long long int raw_add;
    int * zeros = (int *)calloc(S,sizeof(int));
    int z=0;
    struct Address add;
    while(feof(stdin) == 0)
    {
        //z+=1;
        //if (z>20) {break;}
        fscanf(stdin,"%c ",&type);
        fscanf(stdin,"%llx\n",&raw_add);
        //printf("%c 0x%X\n",type,raw_add);
        memset(&add,0,sizeof(struct Address));
        add = process_address(raw_add);
        //printf("%d %X\n",add.index,add.tag);
        moving_cache = cache + BLOCK_SIZE*assoc*add.index - BLOCK_SIZE;
        bool hit = false;
        bool full = true;
        for(int i=0;i<assoc;i++)
        {
            moving_cache+=BLOCK_SIZE;
            if((*moving_cache == add.tag) && *(moving_cache+1) == 1)
            {
                hit = true;
                full = false;
                //printf("%d ",i);
                //printf("Hit at index: %d\n",i);     
                if(!random)
                {
                    auto l = std::next(LRU.begin(),add.index);
                    (*l).remove(i);
                    (*l).push_front(i);
                }
                break;
            }
        }
        //miss
        if(!hit)
        {
            moving_cache = cache + BLOCK_SIZE*assoc*add.index - BLOCK_SIZE;
            
            for(int i=0;i<assoc;i++)
            {
                moving_cache+=BLOCK_SIZE;
                if(*(moving_cache+1) == 0)
                {
                    //if(i!=0) {printf("%d-",i);}
                    // empty
                    //printf("Found empty slot at index: %i\n",i);     
                    *moving_cache = add.tag;
                    *(moving_cache+1) = 1;
                    full = false;
                    if(!random)
                    {
                        auto l = std::next(LRU.begin(),add.index);
                        (*l).remove(i);
                        (*l).push_front(i);
                    }
                    break;
                }
            }
        }
        if(full)
        {
            //printf("FULL\n");
            if(random)
            {
                int index = random_replace(assoc);
                moving_cache = cache + BLOCK_SIZE*assoc*add.index;
                moving_cache += BLOCK_SIZE*index;
                *moving_cache = add.tag;
            }
            else //LRU
            {
                auto l = std::next(LRU.begin(),add.index);
                int removal = (*l).back();
                (*l).pop_back();
                (*l).push_front(removal);
                moving_cache = cache + BLOCK_SIZE*assoc*add.index;
                moving_cache += BLOCK_SIZE*removal;
                *moving_cache = add.tag;

            }
        }
        z+=1;
        if(type == 'r')
        {
            ra+=1;
            if(!hit){rms+=1;}
        }
        else
        {
            wa+=1;
            if(!hit){wms +=1;}
        }
    }   
    ta = ra+wa;
    ms = rms + wms;
    printf("%d %.6f%% %d %.6f%% %d %.6f%%",ms,ms/float(ta)*100.0,rms,rms/float(ra)*100.0,wms,(wms/float(wa))*100.0);

     
}

int main(int argc, char *argv[])
{
    if(argc != 5)
    {
        printf("Usage: nk, assoc, blocksize, repl\n%d\n",argc);
        return 0;
    }
    srand((unsigned) time(NULL));
    //FILE *fd = fopen(argv[1],"r");
    int nk = atoi(argv[1])*1024;
    int assoc = atoi(argv[2]);
    int blocksize = atoi(argv[3]);
    BLOCK_SIZE = blocksize;
    S = nk/(assoc*blocksize);
    char repl = argv[4][0];
    long long int* cache = (long long int*)calloc(nk,sizeof(long long int));
    bool is_random = true;
    if(repl == 'l') {is_random = false;}
    process_instructions(cache,is_random,assoc);
    
    free(cache);
    //fclose(fd);

    
    return 0;
}

