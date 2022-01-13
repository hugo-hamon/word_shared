#ifndef READFILES__H
#define READFILES__H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>

typedef struct files files;

// files_empty: crée une structure de données correspondant initialement
// aux fichier initialiser.Renvoie NULL en cas de dépassement de capacité.
// Renvoie sinon un pointeur vers l'objet qui gère la structure de données.
files *files_empty(size_t size);

// files_update_fp: Modifie la structure f est le tableau associé à la case p
// pour la mettre à jour.
void files_update_fp(files *f, size_t p);

// files_update_occ: Modifie la structutre f pour incrément le nombre
// d'occurence de mot associer à celui-ci.
void files_update_occ(files *f);

// files_get_value: Retourne le nombre d'occurence lié à la structure f
long int files_get_value(files *f);

//files_get_file_number: retourne le nombre de fichier dans lequel apparait le
// mot.
size_t files_get_file_number(files *f);

//files_file_to_str: affiche en fonction de l'appartenance à un fichier un x ou
// un -.
void files_file_to_str(files *f);

// files_dispose: Libére la mémoire lié a une structure file
void files_dispose(files *f);

// is_file: Retourne si s est un chemin vers un fichier. Renvoie true si cela en
// est un sinon false.
bool is_file(char *s);
// is_stream: Retourne si s est une options pour lire sur l'entrée standart.
bool is_stream(char *s);

// Lis dans le ficheir de nom filename stdin et ajoute les mots dans la hastable
// et dans holdall
int add_file_to_hashtable(char *filename, long int initial, bool punctuation,
    bool uppercasing, void *ht, void *has, void *hacptr,
    void *(*hashtable_add)(void *ht, const void *keyptr, const void *valptr),
    void *(*hashtable_search)(void *ht, void *keyptr),
    int(holdall_put) (void *ha, void *ptr), size_t nbfs, size_t nbf);

// Lis sur l'entrée standart stdin et ajouter les mots dans la hastable et dans
// holdall
int add_stream_to_hashtable(long int initial, bool punctuation,
    bool uppercasing, void *ht, void *has, void *hacptr,
    void *(*hashtable_add)(void *ht, const void *keyptr, const void *valptr),
    void *(*hashtable_search)(void *ht, void *keyptr),
    int(holdall_put) (void *ha, void *ptr), size_t nbfs, size_t nbf);

#endif
