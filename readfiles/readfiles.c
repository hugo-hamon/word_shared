#include "readfiles.h"

#define MCHAR 64

struct files {
  long int wc;
  size_t nb_files;
  bool *wf;
};

files *files_empty(size_t nbfs) {
  files *f = malloc(sizeof *f);
  if (f == NULL) {
    return NULL;
  }
  f->wc = 1;
  f->nb_files = nbfs;
  f->wf = malloc(sizeof(bool) * nbfs);
  if (f->wf == NULL) {
    return NULL;
  }
  for (size_t i = 0; i < nbfs; ++i) {
    f->wf[i] = 0;
  }
  return f;
}

void files_update_fp(files *f, size_t p) {
  if (p >= f->nb_files) {
    return;
  }
  f->wf[p] = 1;
}

void files_update_occ(files *f) {
  if (LONG_MAX - f->wc <= 0) {
    return;
  }
  f->wc++;
}

long int files_get_value(files *f) {
  return f->wc;
}

size_t files_get_file_number(files *f) {
  size_t count = 0;
  for (size_t i = 0; i < f->nb_files; i++) {
    count += (f->wf[i] == 1);
  }
  return count;
}

void files_file_to_str(files *f) {
  for (size_t i = 0; i < f->nb_files; i++) {
    if (f->wf[i] == 1) {
      printf("x");
    } else {
      printf("-");
    }
  }
}

void files_dispose(files *f) {
  free(f->wf);
  free(f);
}

bool is_file(char *s) {
  FILE *f = fopen(s, "r");
  if (f == NULL) {
    return false;
  }
  if (!fclose(f)) {
    return true;
  }
  return false;
}

bool is_stream(char *s) {
  if (!strcmp(s, "-")) {
    return true;
  }
  return false;
}

// add_letter_to_word_table: Ajoute au tableau word la lettre c. Dans le cas ou
// le nombre de caractere dépasse la capacité du tableau, le tableau est
// réalloué avec une taille de mchar * 2.
// Renvoie -1 en cas d'erreur 0 sinon.
static int add_letter_to_word_table(char **word, int c, long int nb_char,
    size_t *mchar) {
  char *t = *word;
  t[nb_char - 1] = (char) c;
  if (nb_char == (long int) *mchar) {
    *mchar *= 2;
    char *p = realloc(t, *mchar);
    if (p == NULL) {
      free(t);
      return -1;
    }
    *word = p;
  }
  return 0;
}

// Ajoute word a ht.
static int add_word_to_hastable(char **word, void **ht, void **has,
    void **hacptr,
    void *(*hashtable_add)(void *ht, const void *keyptr, const void *valptr),
    void *(*hashtable_search)(void *ht, void *keyptr),
    int(holdall_put) (void *ha, void *ptr), size_t nbfs, size_t nbf) {
  files *cptr = (files *) hashtable_search(*ht, *word);
  if (cptr != NULL) {
    if (*word == NULL) {
      return -1;
    }
    files_update_occ(cptr);
    files_update_fp(cptr, nbf);
  } else {
    char *s = malloc(strlen(*word) + 1);
    if (s == NULL) {
      return -1;
    }
    strcpy(s, *word);
    if (holdall_put(*has, s) != 0) {
      free(s);
      return -1;
    }
    cptr = files_empty(nbfs);
    if (cptr == NULL) {
      return -1;
    }
    files_update_fp(cptr, nbf);
    if (holdall_put(*hacptr, cptr) != 0) {
      free(cptr);
      return -1;
    }
    if (hashtable_add(*ht, s, cptr) == NULL) {
      return -1;
    }
  }
  return 0;
}

int add_file_to_hashtable(char *filename, long int initial, bool punctuation,
    bool uppercasing, void *ht, void *has, void *hacptr,
    void *(*hashtable_add)(void *ht, const void *keyptr, const void *valptr),
    void *(*hashtable_search)(void *ht, void *keyptr),
    int(holdall_put) (void *ha, void *ptr), size_t nbfs, size_t nbf) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    return -1;
  }
  int c;
  size_t mchar = MCHAR;
  char *word = malloc(mchar);
  if (word == NULL) {
    return -1;
  }
  long int nb_char = 0;
  while ((c = fgetc(file)) != EOF) {
    if ((punctuation && ispunct(c))) {
      if (nb_char >= 1) {
        if (add_letter_to_word_table(&word, '\0', nb_char + 1, &mchar) != 0) {
          return -1;
        }
        if (add_word_to_hastable(&word, &ht, &has, &hacptr, hashtable_add,
            hashtable_search, holdall_put, nbfs, nbf) != 0) {
          return -1;
        }
      }
      nb_char = 0;
      continue;
    }
    if (uppercasing) {
      c = toupper(c);
    }
    nb_char++;
    if (isspace(c) || c == '\0' || nb_char == initial) {
      if (!isgraph(c) && nb_char == 1) {
        nb_char = 0;
        continue;
      }
      if (nb_char == initial) {
        if (!isspace(c)) {
          if (add_letter_to_word_table(&word, c, nb_char, &mchar) != 0) {
            return -1;
          }
        }
        if (add_letter_to_word_table(&word, '\0', nb_char + 1, &mchar) != 0) {
          return -1;
        }
        int c = fgetc(file);
        if (!isspace(c) && c != EOF) {
          if (word == NULL) {
            return -1;
          }
          printf("ws: Word from file '%s' was cut: '%s...'.\n", filename,
              word);
          while (((c = fgetc(file)) != EOF)
              && !isspace(c)) {
          }
        }
        nb_char++;
      } else {
        if (add_letter_to_word_table(&word, '\0', nb_char, &mchar) != 0) {
          return -1;
        }
      }
      nb_char = 0;
      if (add_word_to_hastable(&word, &ht, &has, &hacptr, hashtable_add,
          hashtable_search, holdall_put, nbfs, nbf) != 0) {
        return -1;
      }
    } else {
      if (add_letter_to_word_table(&word, c, nb_char, &mchar) != 0) {
        return -1;
      }
    }
  }
  free(word);
  if (fclose(file) == EOF) {
    return -1;
  }
  return 0;
}

int add_stream_to_hashtable(long int initial, bool punctuation,
    bool uppercasing, void *ht, void *has, void *hacptr,
    void *(*hashtable_add)(void *ht, const void *keyptr, const void *valptr),
    void *(*hashtable_search)(void *ht, void *keyptr),
    int(holdall_put) (void *ha, void *ptr), size_t nbfs, size_t nbf) {
  FILE *f = fopen(".read", "w");
  char buf[initial + 1];
  // Début lecture
  while (fgets(buf, (int) initial, stdin) != NULL) {
    fputs(buf, f);
  }
  rewind(stdin);
  // Fin lecture
  if (fclose(f) == EOF) {
    return -1;
  }
  int r = add_file_to_hashtable((char *) ".read", initial, punctuation,
      uppercasing, ht, has, hacptr, hashtable_add, hashtable_search,
      holdall_put, nbfs, nbf);
  if (r != 0) {
    return -1;
  }
  return remove(".read");
}
