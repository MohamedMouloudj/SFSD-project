#ifndef FLVC_H
#define FLVC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TAILLE_TAILLE_CONTACT 3
#define B 500
#define NOM_FICHIER "Contacts.bin"

//* //// les structures /////////////
typedef struct contact {
	char est_Efface[1+2];
	char ID[8+2];
	char nom[30+2];
	char tel[10+2];
	char adresse_Email[30+2];
	char* observation;
	char taille_Contact[TAILLE_TAILLE_CONTACT+2];
}contact;

typedef struct bloc {
	char contact_Tab[B];
	int suiv;
}bloc, buffer;

typedef struct Entete {
	int adr_Premier_Bloc;
	int nb_Contacts_Insere;
	int nb_Contacts;
	int nb_Contacts_Sup;
	int nb_Blocs;
	int taill_Fichier;
}Entete;

typedef struct LVC {// Nous avons utilisé cette structure pour lire les caractéristiques de la tete une fois durant l'exécution et le develppement du program
	Entete tete;
	FILE* f;
}LVC;
//* ////////////////////////////////////

//
// '|' pour séparer les champs, et '#' pour séparer les contacts
//

//* //// les prototypes ////

//* 0: nb_Blocs, 1: adr_Premier_Bloc, 3: nb_Contacts, 4: nb_Contacts_Insere, 5: nb_Contacts_Sup, 6: taill_Fichier
int entete(LVC *file, int carac_Num);
void affecterEntete(LVC *file, int carac_Num, int val);

void lireBloc(LVC *file, int num_Bloc, buffer *buf);
void ecrireBloc(LVC *file, int num_Bloc, buffer *buf);
LVC *ouvrir(char *nom_Fichier, char mode);
void fermer(LVC *file);

//* Pour allouer un bloc dans le fichier
void allouer_Bloc(LVC *file);

//* préparation du contact dan une chaine pour l'écrire dans le fichier
void creerContact(contact *c,char *contact_A_Insere);

//* Pour générer un contact aléatoirement
void generer_Contact(contact *c);

//* insertion un contact dans le fichier (soit automatiquement ou manuellement)
void inserer(LVC *file, contact *c , bool estAuto, int N,int *current_ID);

//* Pour remplir le fichier avec des contacts aléatoires
void creation_Fichier(LVC *file, int nb_Contacts);

//* pour trouver le bloc et le déplacement d'un contact dans le fichier
char *lire_ID(char *contact,buffer* buf,LVC* file);
void recherche(LVC *file, int id, int *adress_bloc, int *deplacement);

//* pour supprimer un contact du fichier
bool suppression(LVC *file,int id);

//* pour afficher l'entete du fichier
void afficher_Entete(LVC *file);

//* pour afficher tous les blocs du fichier d'une façon simple
void afficher_fichier(LVC *file);

#endif