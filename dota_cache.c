#include "dota_cache.h"

dota_cache_list CreateEmptyCacheList ()
{
	dota_cache_list h;
	h = (dota_cache_list) malloc (sizeof (dota_cache_node));
	h->next = NULL;
	return h;
}


int EmptyCacheList (dota_cache_list h)
{
	return NULL == h->next;
}

dota_cache_list GetCacheNode (dota_cache_list h, dota_cache_list *t)
{
	if (1 == EmptyCacheList (h))
	{
		return NULL;
	}

	dota_cache_list p = h->next;
	h->next = p->next;
	if (p->next == NULL)
	{
		*t = h;
	}
	return p;
}

int InsertCacheNode (dota_cache_list *t, dota_cache_data x)
{
	dota_cache_list q = (dota_cache_list)malloc (sizeof (dota_cache_node));
	if (NULL == q)
	{
		printf ("InsertCacheNode Error\n");
		return -1;
	}
	(*t)->next = q;
	*t = q;
	q->data = x;
	q->next = NULL;

	return 0;
}


