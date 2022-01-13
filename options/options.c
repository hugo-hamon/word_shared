#include "options.h"

#define NBR_OPT 9 // Nombre d'options
#define NBR_HOPT 4 //  Nombre d'options d'aide

struct options {
  long int i; // Options i
  bool p; // Options p
  bool s; // Options s
  long int t; // Options t
  bool u; // Options u
};

options *options_empty(void) {
  options *opts = malloc(sizeof *opts);
  if (opts == NULL) {
    return NULL;
  }
  opts->i = 63;
  opts->p = false;
  opts->s = false;
  opts->t = 10;
  opts->u = false;
  return opts;
}

void options_change_value(options *opts, char *s, long int value) {
  if (!strcmp(s, "-i")) {
    opts->i = (value == 0) ? LONG_MAX : value;
  }
  if (!strcmp(s, "-t")) {
    opts->t = (value == 0) ? LONG_MAX : value;
  }
  if (!strcmp(s, "-p")) {
    opts->p = (value != 0);
  }
  if (!strcmp(s, "-s")) {
    opts->s = (value != 0);
  }
  if (!strcmp(s, "-u")) {
    opts->u = (value != 0);
  }
}

long int options_get_value(options *opts, int n) {
  if (n == 0) {
    return opts->i;
  }
  if (n == 1) {
    return opts->p;
  }
  if (n == 2) {
    return opts->s;
  }
  if (n == 3) {
    return opts->t;
  }
  if (n == 4) {
    return opts->u;
  }
  return -1;
}

void options_dispose(options **opts) {
  if (*opts == NULL) {
    return;
  }
  free(*opts);
  *opts = NULL;
}

bool is_options(const char *s) {
  const char *opt_list[NBR_OPT] = {
    "-i", "-p", "-s", "-t", "-u", "--help", "-?", "--usage", "--version"
  };
  for (int i = 0; i < NBR_OPT; ++i) {
    if (!strcmp(s, opt_list[i])) {
      return true;
    }
  }
  return false;
}

long int string_to_value(char *s) {
  char *endptr;
  errno = 0;
  long int value = strtol(s, &endptr, 10);
  if (endptr == s) {
    return -1;
  }
  if ((value == LONG_MIN || value == LONG_MAX) && errno == ERANGE) {
    return -2;
  }
  if (value < 0) {
    return -1;
  }
  return value;
}

bool is_help_option(const char *s) {
  const char *hopt_list[NBR_HOPT] = {
    "--help", "-?", "--usage", "--version"
  };
  for (int i = 0; i < NBR_HOPT; ++i) {
    if (!strcmp(s, hopt_list[i])) {
      return true;
    }
  }
  return false;
}

void wich_help_options(const char *s) {
  if (!strcmp(s, "--help") || !strcmp(s, "-?")) {
    show_help_option();
  }
  if (!strcmp(s, "--usage")) {
    show_usage_option();
  }
  if (!strcmp(s, "--version")) {
    show_version_option();
  }
}

void show_help_option(void) {
  printf("Usage: ws [OPTION]... FILES\n\n");
  printf("Print a list of words shared by text files.\n\n");
  printf("Mandatory arguments to long options are mandatory for short options"
      " too.\n\n");
  printf("  -i, --initial=VALUE    Set the maximal number of significant"
      " initial letters for words to VALUE. 0 means without limitation."
      " Default is 63.\n\n");
  printf("  -p, --punctuation-like-space    Make the punctuation characters"
      " play the same role as space characters in the meaning of words.\n\n");
  printf("  -s, --same-numbers    Print more words than the limit in case of"
      " same numbers.\n\n");
  printf("  -t, --top=VALUE   Set the maximal number of words to print to"
      " VALUE. 0 means all the words. Default is 10.\n\n");
  printf("  -u, --uppercasing    Convert each lowercase letter of words to the"
      " corresponding uppercase letter.\n\n");
  printf("  -?, --help        Print this help message and exit.\n\n");
  printf("      --usage       Print a short usage message and exit.\n\n");
  printf("      --version     Print version information.\n\n");
  printf("The number of FILES that can be taken into account is between 2 and"
      " 32. The counters of the total number of occurrences of the words have"
      " a maximum value of 9223372036854775807.\n");
}

void show_usage_option(void) {
  printf("Usage: ws [OPTION]... FILES\n");
}

void show_version_option(void) {
  printf("ws - 2022/01/01\n");
  printf("This is freeware: you can redistribute it. There is NO WARRANTY.\n");
}

void error_option_argument(char *s) {
  fprintf(stderr, "ws: Invalid argument '%s'\n", s);
  printf("Try 'ws --help' for more information.\n");
}

void error_overflow_argument(char *s) {
  fprintf(stderr, "ws: Overflowing argument '%s'\n", s);
  printf("Try 'ws --help' for more information.\n");
}

void error_number_of_arguments(void) {
  fprintf(stderr, "ws: At least 2 files are expected.\n");
  printf("Try 'ws --help' for more information.\n");
}

void error_exceeding_capacity_file(void) {
  fprintf(stderr, "ws: Number of files exceeding capacity\n");
  printf("Try 'ws --help' for more information.\n");
}

void error_cant_reading_file(char *s) {
  fprintf(stderr, "ws: Can't open for reading file '%s'.\n", s);
}
