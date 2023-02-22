#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "cachelab.h"
#define INF 100000
struct block
{
    int valid, tag, lru;
};
struct Cache
{
    int S, E, b;
    struct block ***store;
};
struct Cache cache;
int hit = 0, miss = 0, evict = 0;
bool wrong = 0;
void set_cache(int s, int e, int _b)
{
    cache.S = s;
    cache.E = e;
    cache.b = _b;
}
void build_cache()
{
    cache.store = (struct block ***)malloc(cache.S * sizeof(struct block **));
    for (int i = 0; i < cache.S; i++)
        cache.store[i] = (struct block **)malloc(cache.E * sizeof(struct block *));
    for (int i = 0; i < cache.S; i++)
        for (int j = 0; j < cache.E; j++)
        {
            cache.store[i][j] = (struct block *)malloc(3 * sizeof(int));
        }
    for (int i = 0; i < cache.S; i++)
        for (int j = 0; j < cache.E; j++)
        {
            cache.store[i][j]->valid = 0;
            cache.store[i][j]->tag = 0;
            cache.store[i][j]->lru = INF;
        }
}
void free_cache()
{
    for (int i = 0; i < cache.S; i++)
        for (int j = 0; j < cache.E; j++)
            free(cache.store[i][j]);
    for (int i = 0; i < cache.E; i++)
        free(cache.store[i]);
    free(cache.store);
}
int judge_hit(int s, int tag)
{
    for (int i = 0; i < cache.E; i++)
    {
        if (cache.store[s][i]->valid == 1 && cache.store[s][i]->tag == tag)
            return i;
    }
    return -1;
}
int find_lru(int s)
{
    int _min = INF;
    int choose = 0;
    for (int i = 0; i < cache.E; i++)
    {
        if (cache.store[s][i]->valid == 0)
            return i;
        if (_min > cache.store[s][i]->lru)
        {
            _min = cache.store[s][i]->lru;
            choose = i;
        }
    }
    return choose;
}
void replace_block(int s, int e, int tag)
{
    if (cache.store[s][e]->valid == 1 && cache.store[s][e]->tag != tag)
        evict++;
    cache.store[s][e]->valid = 1;
    cache.store[s][e]->tag = tag;
    cache.store[s][e]->lru = INF;
    for (int i = 0; i < cache.E; i++)
        if (i != e)
            cache.store[s][i]->lru--;
}
void show_cache()
{
    for (int i = 0; i < cache.S; i++)
    {
        for (int j = 0; j < cache.E; j++)
            printf("%d,%d,%d     ", cache.store[i][j]->valid, cache.store[i][j]->tag, cache.store[i][j]->lru);
        printf("\n");
    }
}
void print_help()
{
    printf("Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
    printf("• - h : Print this help\n");
    printf("• - v : Optional verbose flag that displays trace info\n");
    printf("• - s<s> : Number of set index bits(S = 2s is the number of sets)\n");
    printf("• - E<E> : Associativity(number of lines per set)\n");
    printf("• - b<b> : Number of block bits(B = 2b is the block size)\n");
    printf("• - t<tracefile> : Name of the valgrind trace to replay\n ");
}
void call(int tag, int S)
{
    if (judge_hit(S, tag) != -1)
    {
        hit++;
        replace_block(S, judge_hit(S, tag), tag);
        return;
    }
    else
    {
        miss++;
        replace_block(S, find_lru(S), tag);
        return;
    }
}
int main(int argc, char *const argv[])
{
    char opt;
    FILE *path = NULL;
    char *filepath = NULL;
    int _s = -1, _e = -1, _b = -1;
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        printf("%c\n", opt);
        switch (opt)
        {

        case 'h':
            print_help();
            break;
        case 'v':
            break;
        case 's':
            _s = atol(optarg);
            //printf("%d\n", _s);
            break;
        case 'E':
            _e = atol(optarg);
            //printf("%d\n", _e);
            break;
        case 'b':
            _b = atol(optarg);
            //printf("%d\n", _b);
            break;
        case 't':
            filepath = optarg;
            printf("%s\n", optarg);
            break;
        default:
            wrong = 1;
        }
    }

    //printf("%d\n",wrong);
    if (_s <= 0 || _e <= 0 || _b <= 0 || filepath == NULL || wrong)
    {
        print_help();
        return 0;
    }
    //printf("%d\n",wrong);
    set_cache(1 << _s, _e, 1 << _b);
    //printf("%d\n",wrong);
    build_cache();
    //printf("%d\n",wrong);
    show_cache();
    long unsigned int address;
    unsigned long number;
    char waste;
    char cmd[2] = "I";
    path = fopen(filepath, "r");
    while (fscanf(path, "%s%lx%c%lu", cmd, &address, &waste, &number) != EOF)
    {
        printf("%d\n", wrong);
        printf("%c %lx%c%lu", cmd[0], address, waste, number);
        if (cmd[0] == 'I')
            continue;
        int tag = address >> (_s + _b);
        int S = (address & ((1 << (_s + _b)) - 1)) >> _b;
        if (cmd[0] == 'M')
        {
            call(tag, S);
            call(tag, S);
        }
        else
            call(tag, S);
        printf("\n");
    }
    printSummary(hit, miss, evict);
    free_cache();

    return 0;
}
