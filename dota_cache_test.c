#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "dota_cache.h"

struct charpack
{
	char a;
	char b;
	char c;
};

struct intpack
{
	int a;
	int b;
	int c;
};

int main ()
{
	dota_cache_list hc = CreateEmptyCacheList ();
	dota_cache_list tc = hc;

	dota_cache_list hi = CreateEmptyCacheList ();
	dota_cache_list ti = hi;

	dota_cache_list temp;

	struct charpack *pc ;
	pc = (struct charpack *) malloc (sizeof (struct charpack));
	pc->a = 'a';
	pc->b = 'b';
	pc->c = 'c';

	struct intpack *pi;
	pi = (struct intpack *) malloc (sizeof (struct intpack));
	pi->a = 1;
	pi->b = 2;
	pi->c = 3;

	InsertCacheNode (&tc, pc);
	InsertCacheNode (&ti, pi);

	pc = (struct charpack *) malloc (sizeof (struct charpack));
	pc->a = 'd';
	pc->b = 'e';
	pc->c = 'f';

	pi = (struct intpack *) malloc (sizeof (struct intpack));
	pi->a = 4;
	pi->b = 5;
	pi->c = 6;

	InsertCacheNode (&tc, pc);
	InsertCacheNode (&ti, pi);

	pc = (struct charpack *) malloc (sizeof (struct charpack));
	pc->a = 'g';
	pc->b = 'h';
	pc->c = 'i';

	pi = (struct intpack *) malloc (sizeof (struct intpack));
	pi->a = 7;
	pi->b = 8;
	pi->c = 9;

	InsertCacheNode (&tc, pc);
	InsertCacheNode (&ti, pi);
	printf ("Finished Insert\n");


	temp = GetCacheNode (hc, &tc);
	pc = (struct charpack *)temp->data;
	printf ("%c, %c, %c\n", pc->a, pc->b, pc->c);
	free (temp);
	free (pc);
	temp = GetCacheNode (hc, &tc);
	pc = (struct charpack *)temp->data;
	printf ("%c, %c, %c\n", pc->a, pc->b, pc->c);
	free (temp);
	free (pc);
	temp = GetCacheNode (hc, &tc);
	pc = (struct charpack *)temp->data;
	printf ("%c, %c, %c\n", pc->a, pc->b, pc->c);
	free (temp);
	free (pc);

	printf ("%p, %p, %p, %p\n", hi, ti, hi->next, ti->next);
	temp = GetCacheNode (hi, &ti);
	pi = (struct intpack *)temp->data;
	printf ("%d, %d, %d\n", pi->a, pi->b, pi->c);
	free (temp);
	free (pi);
	printf ("%p, %p, %p, %p\n", hi, ti, hi->next, ti->next);
	temp = GetCacheNode (hi, &ti);
	pi = (struct intpack *)temp->data;
	printf ("%d, %d, %d\n", pi->a, pi->b, pi->c);
	free (temp);
	free (pi);
	printf ("%p, %p, %p, %p\n", hi, ti, hi->next, ti->next);
	temp = GetCacheNode (hi, &ti);
	pi = (struct intpack *)temp->data;
	printf ("%d, %d, %d\n", pi->a, pi->b, pi->c);
	free (temp);
	free (pi);
	printf ("%p, %p, %p, %p\n", hi, ti, hi->next, ti->next);

	pi = (struct intpack *) malloc (sizeof (struct intpack));
	pi->a = 7;
	pi->b = 8;
	pi->c = 9;

	InsertCacheNode (&ti, pi);

	temp = GetCacheNode (hi, &ti);
	pi = (struct intpack *)temp->data;
	printf ("%d, %d, %d\n", pi->a, pi->b, pi->c);
	free (temp);
	free (pi);
	printf ("%p, %p, %p, %p\n", hi, ti, hi->next, ti->next);
	return 0;
}
