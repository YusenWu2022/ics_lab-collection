#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "cachelab.h"

//定义结构体
typedef struct {
	long tag; //标识
	long time;  //时间戳
	int valid_bit;  //有效位
} Line;

//创建缓存
Line **initCache(int s, int E){
	Line **cache;
	int i,j;
	int S = 1 << s; //2^s
	cache = (Line **)malloc(S*sizeof(Line*));
	if(cache==NULL) exit(1);
	for(i=0;i<S;i++){
		cache[i] = (Line*)malloc(E*sizeof(Line));
		for(j=0;j<E;j++){
			cache[i][j].tag=0;
			cache[i][j].time=0;
			cache[i][j].valid_bit=0;
		}
	}
	return cache;
}
int main(int argc, char** argv){
	int opt,s,E,b;
	int S,tag;  //解析每一条对应的组号和标志
	char *fileName;
	Line **cache;
	int hit_count=0, miss_count=0, eviction_count=0;
	int isHit, isEvic;  //标记是否命中或驱逐
	FILE *pFile;
	char identifier;
	unsigned long long address;
	int size;
	int saveIndex;
	long time_stamp=0;    //不断增加的时间戳
	long min_time;  //LRU
	int index;

	//读取参数
	while(-1 != (opt = getopt(argc, argv, "s:E:b:t:"))){
		switch(opt){
			case 's':
				s = atoi(optarg);
				break;
			case 'E':
				E = atoi(optarg);
				break;
			case 'b':
				b = atoi(optarg);
				break;
			case 't':
				fileName = (char *)optarg;
			default:
				printf("wrong argument\n");
				break;
		}
	}

	//初始化缓存
	cache = initCache(s, E);

	pFile = fopen(fileName, "r");
	while(fscanf(pFile," %c %llx,%d", &identifier, &address, &size)>0){
		if(identifier == 'I') continue; //不考虑指令读取

		//获取当前的组号和标志
		address = address >> b;
		S = address & ~(~0 << s);
		tag = address >> s;

		isHit = 0;
		isEvic = 0;
		min_time = LONG_MAX;

		for(index=0; index<E; index++){
			Line line = cache[S][index];
			if(line.valid_bit==0){
				saveIndex = index;
				min_time = LONG_MIN;
				isEvic = 0;
				continue;
			}
			if(line.tag!=tag){
				if(min_time > line.time){
					min_time = line.time;
					saveIndex = index;
					isEvic = 1;
				}
				continue;
			}
			saveIndex = index;
			isHit = 1;
			break;
		}
		cache[S][saveIndex].time = time_stamp;
		time_stamp += 1;
		if(identifier == 'M') hit_count += 1;
		if(isHit){
            hit_count += 1;
            continue;
        }
        miss_count += 1;
        if(isEvic) eviction_count += 1;
        cache[S][saveIndex].valid_bit = 1;
        cache[S][saveIndex].tag = tag;
	}
	free(cache);
	printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
