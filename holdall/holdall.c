//  Partie implantation du module holdall.

#include "holdall.h"

//  struct holdall, holdall : implantation par liste dynamique simplement
//    chainée. L'insertion a lieu en queue si la macroconstante
//    HOLDALL_INSERT_TAIL est définie, en tête sinon.

typedef struct choldall choldall;

struct choldall {
  void *value;
  choldall *next;
};

struct holdall {
  choldall *head;
#ifdef HOLDALL_INSERT_TAIL
  choldall *tail;
#endif
  size_t count;
};

holdall *holdall_empty(void) {
  holdall *ha = malloc(sizeof *ha);
  if (ha == NULL) {
    return NULL;
  }
  ha->head = NULL;
#ifdef HOLDALL_INSERT_TAIL
  ha->tail = NULL;
#endif
  ha->count = 0;
  return ha;
}

int holdall_put(holdall *ha, void *ptr) {
  choldall *p = malloc(sizeof *p);
  if (p == NULL) {
    return -1;
  }
  p->value = ptr;
#ifdef HOLDALL_INSERT_TAIL
  p->next = NULL;
  if (ha->tail == NULL) {
    ha->head = p;
  } else {
    ha->tail->next = p;
  }
  ha->tail = p;
#else
  p->next = ha->head;
  ha->head = p;
#endif
  ha->count += 1;
  return 0;
}

size_t holdall_count(holdall *ha) {
  return ha->count;
}

int holdall_apply(holdall *ha, int (*fun)(void *)) {
  for (const choldall *p = ha->head; p != NULL; p = p->next) {
    int r = fun(p->value);
    if (r != 0) {
      return r;
    }
  }
  return 0;
}

int holdall_apply_context(holdall *ha,
    void *context, void *(*fun1)(void *context, void *ptr),
    int (*fun2)(void *ptr, void *resultfun1)) {
  for (const choldall *p = ha->head; p != NULL; p = p->next) {
    int r = fun2(p->value, fun1(context, p->value));
    if (r != 0) {
      return r;
    }
  }
  return 0;
}

int holdall_apply_context2(holdall *ha,
    void *context1, void *(*fun1)(void *context1, void *ptr),
    void *context2, int (*fun2)(void *context2, void *ptr, void *resultfun1)) {
  for (const choldall *p = ha->head; p != NULL; p = p->next) {
    int r = fun2(context2, p->value, fun1(context1, p->value));
    if (r != 0) {
      return r;
    }
  }
  return 0;
}

void holdall_dispose(holdall **haptr) {
  if (*haptr == NULL) {
    return;
  }
  choldall *p = (*haptr)->head;
  while (p != NULL) {
    choldall *t = p;
    p = p->next;
    free(t);
  }
  free(*haptr);
  *haptr = NULL;
}

// prev: Renvoie l'élément precedent à p. Renvoie l'élément precedent si il
// existe sinon NULL.
choldall *prev(choldall *head, choldall *p) {
  if (head == p) {
    return NULL;
  }
  choldall *tmp = head;
  choldall *pre = NULL;
  while (tmp != NULL && tmp != p) {
    pre = tmp;
    tmp = tmp->next;
  }
  if (tmp != p) {
    return NULL;
  }
  return pre;
}

// swap: permute p1 et p2.
void swap(choldall **head, choldall **p1, choldall **p2) {
  if (*head == NULL || *p1 == NULL || *p2 == NULL) {
    return;
  }
  choldall *pp1 = prev(*head, *p1);
  choldall *pp2 = prev(*head, *p2);
  if (pp1 != NULL) {
    pp1->next = (*p2);
  }
  if (pp2 != NULL) {
    pp2->next = (*p1);
  }
  choldall *tmp = (*p1)->next;
  (*p1)->next = (*p2)->next;
  (*p2)->next = tmp;
  if (*head == *p1) {
    *head = *p2;
  } else {
    if (*head == *p2) {
      *head = *p1;
    }
  }
}

// Separe src en deux.
void split(choldall *src, choldall **front, choldall **back) {
  choldall *s = src;
  choldall *f = src->next;
  while (f != NULL) {
    f = f->next;
    if (f != NULL) {
      s = s->next;
      f = f->next;
    }
  }
  *back = s->next;
  *front = src;
  s->next = NULL;
}

// Fusionne a et b
choldall *merge(choldall *a, choldall *b, void *context, int (*compar)(
    const void *, const void *, const void *)) {
  choldall *res = NULL;
  if (a == NULL || b == NULL) {
    return (a == NULL) ? b : a;
  }
  if ((compar(context, a->value, b->value)) < 0) {
    res = b;
    res->next = merge(a, b->next, context, compar);
  } else {
    res = a;
    res->next = merge(a->next, b, context, compar);
  }
  return res;
}

// Trie la liste chainée selon merge sort.
void merge_sort(choldall **head, void *context, int (*compar)(const void *,
    const void *, const void *)) {
  choldall *hd = *head;
  choldall *a;
  choldall *b;
  // Cas d'un seul élément ou de 0
  if (hd == NULL || hd->next == NULL) {
    return;
  }
  split(hd, &a, &b);
  merge_sort(&a, context, compar);
  merge_sort(&b, context, compar);
  *head = merge(a, b, context, compar);
}

int holdall_sort(holdall *ha, void *context, int (*compar)(const void *,
    const void *, const void *)) {
  merge_sort(&(ha->head), context, compar);
  return 0;
}
