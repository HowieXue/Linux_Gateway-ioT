#ifndef __DOTA_CACHE_H__
#define __DOTA_CACHE_H__

#include <stdio.h>
#include <stdlib.h>

typedef void* dota_cache_data;

typedef struct _dota_node_
{
	dota_cache_data data;
	struct _dota_node_ *next;
}dota_cache_node, *dota_cache_list;

extern dota_cache_list CreateEmptyCacheList ();
extern int EmptyCacheList (dota_cache_list);
extern dota_cache_list GetCacheNode (dota_cache_list, dota_cache_list *);
extern int InsertCacheNode (dota_cache_list *, dota_cache_data);

#endif
