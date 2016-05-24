#include "link_list.h"
#include "data_global.h"

linklist linkHead, linkTail;

linklist CreateEmptyLinklist ()
{
	linklist h;
	h = (linklist)malloc (sizeof (linknode));
	linkTail = h;
	h->next = NULL;
	return h;
}

int EmptyLinklist (linklist h)
{
	return NULL == h->next;
}

linklist GetLinknode (linklist h)
{
	if (1 == EmptyLinklist (h))	
	{
		return NULL;
	}
	linklist p = h->next;
	h->next = p->next;
	if (p->next == NULL)
		linkTail = h;
	return p;
}

int InsertLinknode (link_datatype x)
{
	linklist q = (linklist)malloc (sizeof (linknode));
	if (NULL == q)
	{
		printf ("InsertLinknode Error\n");
		return -1;
	}
	linkTail->next = q;
	linkTail = q;
	q->data = x;
	q->next = NULL;

	return 0;
}


