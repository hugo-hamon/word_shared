#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "options.h"
#include "readfiles.h"
#include "hashtable.h"
#include "holdall.h"

//  str_hashfun : une fonction de pré-hachage pour les chaines de caractères.
static size_t str_hashfun(const char *s);

//  scptr_display : affiche sur la sortie standard *cptr, le caractère
//    tabulation, la chaine de caractères pointée par s et la fin de ligne.
//    Renvoie zéro en cas de succès, une valeur non nulle en cas d'échec.
static int scptr_display(options *i, const char *s, files *cptr);

//  wfree : libère la zone mémoire pointée par ptr et renvoie zéro.
static int wfree(void *ptr);

//  ffree : libère la zone mémoire pointée par ptr et renvoie zéro.
static int ffree(void *ptr);

// compar_fun: compare f1 et f2 au sens de compar.
int compar_fun(hashtable *ht, char *w1, char *w2);

int main(int argc, char *argv[]) {
  int r = EXIT_SUCCESS;
  size_t nbr_file = 0;
  // Création des fourres tout et de la table de hachage
  hashtable *ht = hashtable_empty((int (*)(const void *, const void *))strcmp,
      (size_t (*)(const void *))str_hashfun);
  holdall *has = holdall_empty();
  holdall *hacptr = holdall_empty();
  if (ht == NULL
      || has == NULL
      || hacptr == NULL) {
    goto dispose;
  }
  // Cas manque d'arguments
  if (argc <= 2) {
    if (argc != 1 && is_help_option(argv[1])) {
      wich_help_options(argv[1]);
      goto exit;
    }
    error_number_of_arguments();
    r = EXIT_FAILURE;
    goto exit;
  }
  options *opts_struct = options_empty();
  if (opts_struct == NULL) {
    goto exit;
  }
  // Lecture des options et test sur les options
  for (int i = 1; i < argc; i++) {
    if (is_options(argv[i])) {
      if (is_help_option(argv[i])) {
        wich_help_options(argv[i]);
        goto dispose;
      }
      if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "-t")) {
        if (i + 1 >= argc) {
          error_option_argument(argv[i]);
          goto dispose_failure;
        }
        long int str_val = string_to_value(argv[i + 1]);
        if (str_val == -1) {
          error_option_argument(argv[i]);
          goto dispose_failure;
        }
        if (str_val == -2) {
          error_overflow_argument(argv[i]);
          goto dispose_failure;
        }
        options_change_value(opts_struct, argv[i], str_val);
        ++i;
      } else {
        options_change_value(opts_struct, argv[i], 1);
      }
    } else if (is_file(argv[i]) || is_stream(argv[i])) {
      if (nbr_file + 1 > 32) {
        error_exceeding_capacity_file();
        goto dispose_failure;
      }
      nbr_file++;
    } else {
      error_cant_reading_file(argv[i]);
      goto dispose_failure;
    }
  }
  // Teste nombre de fichier
  if (nbr_file < 2) {
    error_number_of_arguments();
    goto dispose_failure;
  }
  // Lecture des fichiers
  size_t nbf = 0;
  int rv = 0;
  for (int i = 1; i < argc; i++) {
    if (is_file(argv[i])) {
      rv = add_file_to_hashtable(argv[i], options_get_value(opts_struct, 0),
          options_get_value(opts_struct, 1),
          options_get_value(opts_struct, 4),
          ht, has, hacptr,
          (void *(*)(void *, const void *, const void *))hashtable_add,
          (void *(*)(void *, void *))hashtable_search,
          (int (*)(void *, void *))holdall_put, nbr_file, nbf);
    } else if (is_stream(argv[i])) {
      rv = add_stream_to_hashtable(options_get_value(opts_struct, 0),
          options_get_value(opts_struct, 1),
          options_get_value(opts_struct, 4),
          ht, has, hacptr,
          (void *(*)(void *, const void *, const void *))hashtable_add,
          (void *(*)(void *, void *))hashtable_search,
          (int (*)(void *, void *))holdall_put, nbr_file, nbf);
    }
    if (rv != 0) {
      printf("Error while reading file\n");
      goto dispose_failure;
    }
    nbf += 1;
  }
  if (holdall_sort(has, ht,
      (int (*)(const void *, const void *, const void *))compar_fun) != 0) {
    goto dispose_failure;
  }
  if (holdall_apply_context2(has,
      ht, (void *(*)(void *, void *))hashtable_search, opts_struct,
      (int (*)(void *, void *, void *))scptr_display) != 0) {
    goto dispose_failure;
  }
  goto dispose;
  // fin
dispose_failure:
  r = EXIT_FAILURE;
  goto dispose;
dispose:
  options_dispose(&opts_struct);
  goto exit;
exit:
  hashtable_dispose(&ht);
  if (has != NULL) {
    holdall_apply(has, wfree);
  }
  holdall_dispose(&has);
  if (hacptr != NULL) {
    holdall_apply(hacptr, ffree);
  }
  holdall_dispose(&hacptr);
  return r;
}

size_t str_hashfun(const char *s) {
  size_t h = 0;
  for (const unsigned char *p = (const unsigned char *) s; *p != '\0'; ++p) {
    h = 37 * h + *p;
  }
  return h;
}

int wfree(void *ptr) {
  free(ptr);
  return 0;
}

int scptr_display(options *i, const char *s, files *cptr) {
  long int c = options_get_value(i, 3);
  const char *ch = "-t";
  if (((c - 1 >= 1) && files_get_file_number(cptr) > 1)
      || files_get_value(cptr) == -c) {
    options_change_value(i, (char *) ch, c - 1);
    files_file_to_str(cptr);
    return printf("\t%ld\t%s\n", files_get_value(cptr), s) < 0;
  } else {
    if (c - 1 == 0) {
      options_change_value(i, (char *) ch, -files_get_value(cptr));
      files_file_to_str(cptr);
      return printf("\t%ld\t%s\n", files_get_value(cptr), s) < 0;
    }
    return 0;
  }
}

int ffree(void *ptr) {
  files_dispose(ptr);
  return 0;
}

int compar_fun(hashtable *ht, char *w1, char *w2) {
  files *f1 = (files *) hashtable_search(ht, w1);
  files *f2 = (files *) hashtable_search(ht, w2);
  if (f1 == NULL || f2 == NULL) {
    return 0;
  }
  size_t nbf1 = files_get_file_number(f1);
  size_t nbf2 = files_get_file_number(f2);
  if (nbf1 == nbf2) {
    long int wc1 = files_get_value(f1);
    long int wc2 = files_get_value(f2);
    if (wc1 == wc2) {
      return strcmp(w2, w1);
    } else {
      return (wc1 > wc2) - (wc1 < wc2);
    }
  } else {
    return (nbf1 > nbf2) - (nbf1 < nbf2);
  }
}
