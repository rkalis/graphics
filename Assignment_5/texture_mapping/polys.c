/* Computer Graphics
 *
 * Filename ........ polys.c
 * Description ..... Functions to manage lists of polygons
 * Date ............ 19.08.2008
 * Created by ...... Jurgen Sturm
 * Cleaned up by ... Paul Melis
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "polys.h"

/* Create a list of polys, and  immediately reserve space for `n' polys */

polys*
CreatePolylist(int n)
{
    polys *l;

    l = (polys*) malloc(sizeof(polys));

    if (l == NULL)
    {
        printf("CreatePolylist(): could not allocate memory (l): %s\n",
           strerror(errno));
        exit(-1);
    }

    if (n > 0)
    {
        l->items = (poly*) malloc(n * sizeof(poly));

        if (l->items == NULL)
        {
            printf("CreatePolylist(): could not allocate memory (data): %s\n",
                strerror(errno));
            exit(-1);
        }

        l->capacity = n;
        l->length = 0;
    }
    else
    {
        l->capacity = 0;
        l->length = 0;
        l->items = NULL;
    }

    return l;
}


void
DestroyPolylist(polys *list)
{
    if (list == NULL)
        return;

    list->capacity = 0;
    list->length = 0;
    free(list->items);

    list = NULL;
}

void
ClearPolylist(polys *list)
{
    list->length = 0;
}

/* exit() if no memory available, returns 0 if poly* is NULL, 1 otherwise */
static int
GrowPolylist(polys *list, int delta)
{
    poly    *newdata;

    if (list == NULL)
        return 0;

    newdata = (poly*) realloc(list->items, (list->capacity + delta)*sizeof(poly));

    if (newdata == NULL)
    {
        printf("GrowPolylist(): could not allocate memory: %s\n",
            strerror(errno));
        exit(-1);
    }

    list->capacity += delta;
    list->items = newdata;

    return 1;
}


/* exit() if no memory available, returns 0 if poly* is NULL, 1 otherwise.
 * Decreasing the data segment can, and will, destroy polygons if the new
 * segment cannot contain all current data.
 * The data segment cannot be reduced to a size less than 0 (size is clamped
 * to zero).
 * The new data segment will be a NULL pointer if the new size is 0. */
static int
ShrinkPolylist(polys *list, int delta)
{
    int     n;
    poly    *newdata;

    if (list == NULL)
        return 0;

    n = list->capacity - delta;
    if (n < 1)
    {
        free(list->items);
        list->items = NULL;
        list->capacity = 0;
        list->length = 0;

        return 1;
    }

    newdata = (poly*)realloc(list->items, (list->capacity + delta)*sizeof(poly));

    if (newdata == NULL)
    {
        printf("ShrinkPolylist(): could not allocate memory: %s\n",
            strerror(errno));
        exit(-1);
    }

    list->capacity -= delta;
    list->items = newdata;

    /* update list->length if neccesary */
    if (list->length > list->capacity)
        list->length = list->capacity;

    return 1;
}


/* poly's are structs of (point) arrays, so they are passed by value instead
 * of by reference (as do arrays) */
void
AddPolyToPolylist(polys *list, poly p)
{
    if (list == NULL)
        return;

    /* polylist is full, so first add some space */
    if (list->length == list->capacity)
    {
        /* grow arbitrary amount */
        if (GrowPolylist(list, 8) != 1)
        {
            printf("AddPolyToList(): failed");
            exit(-1);
        }
    }

    list->items[list->length] = p;
    list->length++;
}

/* Append the items of 'to_append' to 'list' */
void
AppendPolylist(polys *list, polys *to_append)
{
    for (int i = 0; i < to_append->length; i++)
        AddPolyToPolylist(list, to_append->items[i]);
}

polys*
CopyPolylist(polys* list)
{
    int     i;
    polys   *copy;

    if (list == NULL)
        return NULL;

    copy = CreatePolylist(list->capacity);

    /* not the most efficient, but certainly easiest & least error-prone */
    for (i = 0; i < list->length; i++)
        AddPolyToPolylist(copy, list->items[i]);

    return copy;
}


