#ifndef OPTIONS__H
#define OPTIONS__H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>

//options: Champs i, p, s, t, u
typedef struct options options;

// options_empty: crée une structure de données correspondant initialement
// aux options initialiser.Renvoie NULL en cas de dépassement de capacité.
// Renvoie sinon un pointeur vers l'objet qui gère la structure de données.
options *options_empty(void);

// options_change_value: Change la valeur d'un champ s dans la structure opts.
// Dans le cas ou s ne correspond à aucun champ opts ou si la valeur est
// incohérente renvoie -1 sinon 0;
void options_change_value(options *opts, char *s, long int value);

// options_get_value: Renvoie la valeur du n éme champs dans la structure opts.
// Dans le cas ou s ne correspond à aucun champ d'opts renvoie -1 sinon la
// valeur du champ.
long int options_get_value(options *opts, int n);

//  options_dispose : si *opts ne vaut pas NULL, libère les ressources
//    allouées à la structure de données associée à *htptr puis affecte à *htptr
//    la valeur NULL.
void options_dispose(options **opts);

// is_option: retourne si une chaine de caratére correspond à une options. Dans
// le cas ou cela en est une retourne true sinon false.
bool is_options(const char *s);

// string_to_value: converti uen chaine de nombre en nombre. Dans le cas ou la
// chaine ne contient pas de nombre retourne -1 ou si le nombre dépasse la
// capacité au sens de LONG_MAX ou LONG_MIN retourn -2 sinon le nombre.
long int string_to_value(char *s);

// is_help_option: retourne si une chaine de caratére correspond à une options.
// Dans le cas ou cela en est une retourne true sinon false.
bool is_help_option(const char *s);

// wich_help_options: Appelle une fonction relative à s;
void wich_help_options(const char *s);

// show_help_option: Affiche l'aide relative à --help ou -?
void show_help_option(void);

// show_usage_option: Affiche l'aide relative à --usage.
void show_usage_option(void);

// show_version_option: Affiche l'aide relative à --version.
void show_version_option(void);

// error_option_argument: Affiche les erreurs relatives à un manque
// d'argument pour une option s.
void error_option_argument(char *s);

// error_overflow_argument: Affiche les erreurs relatives à un depassement de
// capacité pour une option s.
void error_overflow_argument(char *s);

// error_number_of_arguments: Affiche les erreurs relatives à un manque
// d'argument sur l'entrée standar.
void error_number_of_arguments(void);

// error_exceeding_capacity_file: Affiche les erreurs relatives à une quantité
// de fichier supérieur à 32.
void error_exceeding_capacity_file(void);

// error_cant_reading_file: Affiche les erreurs relatives à une impossibilité
// d'ouverture d'un fichier.
void error_cant_reading_file(char *s);

#endif
