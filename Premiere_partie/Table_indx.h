#ifndef TABLE_INDEX_H
#define TABLE_INDEX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "texteColor.h"
#include "flvc.h"


#define MAX_INDEX_TAILLE 500
#define NOM_FICHIERI_INDEX "Contacts_Index.bin"
 
typedef struct TableIndex {
    int ID;         //cle
    int Bloc;       //adresse bloc
    int Offset;     //adresse offset
}TableIndex;

typedef struct FileIndex{
    TableIndex tab[MAX_INDEX_TAILLE];
    int taille;
    FILE *fileIndex;
} FileIndex;

FileIndex *ouvrirT(char mode);
void fermerT(FileIndex *file);
void afficherIndex(FileIndex *file);
int rechercheCle(FileIndex *file, int cle);
void suppressionEntre(FileIndex *file, int cle);
void insertionEntre(FileIndex *file, int cle, int adr_B, int adr_E);
void trierIndex(FileIndex *file);
void remplirIndex(FileIndex *file, LVC *fileLVC,int nbreGenere,int nbreInsereIDs,int insereIDs[]);
void afficherContact(FileIndex *file, int i);
void afficherContactParCle(FileIndex *file, int cle);
void Lire_Contact_Fichier(LVC* fileLVC,buffer buf,int adr_B,int adr_E,char *contact);

#endif