#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Table_indx.h"

#define BUFFER_SIZE 1024
extern char *strtok_s(char *, const char *, char **);


FileIndex *ouvrirT(char mode){
	FileIndex *file = malloc(sizeof(FileIndex));
	if(file == NULL){
		color(1);
		printf("\tErreur d'allocation de mémoire\n");
		resetClor();
		return NULL;
	}
	if(mode == 'w'){
		file->fileIndex = fopen(NOM_FICHIERI_INDEX, "wb+");
		file->taille = 0;		
		if(file->fileIndex == NULL){
			color(1);
			printf("\tErreur d'ouverture du fichier %s\n", NOM_FICHIERI_INDEX);
			resetClor();
			return NULL;	
		}
	}else{
		file->fileIndex = fopen(NOM_FICHIERI_INDEX, "rb+");
		if(file->fileIndex == NULL){
			color(1);
			printf("\tErreur d'ouverture du fichier %s\n", NOM_FICHIERI_INDEX);
			resetClor();
			return NULL;
		}
		fread(&(file->taille), sizeof(int), 1, file->fileIndex);
		fread(file->tab, sizeof(TableIndex), file->taille, file->fileIndex);
	}
	return file;
}

void fermerT(FileIndex *file){
	fwrite(&(file->taille), sizeof(int), 1, file->fileIndex);
	fwrite(file->tab, sizeof(TableIndex), file->taille, file->fileIndex);
	fclose(file->fileIndex);
	free(file);
}

// Recherche dichotomique d'un couple (ID, adresse) dans l'index (recherche d'une entrée)
int rechercheCle(FileIndex *file, int cle){
	int debut=0;
	int fin=file->taille-1;
	int milieu;

	while(debut <= fin){
		milieu = (debut + fin) / 2;
		if(file->tab[milieu].ID == cle){
			return milieu;
		}
		else if(file->tab[milieu].ID < cle){
			debut = milieu + 1;
		}
		else{
			fin = milieu - 1;
		}
	}
	return -1;
}

void afficherIndex(FileIndex *file){
	int i;
	for(i = 0; i < file->taille; i++){
		printf("ID: %d\n", file->tab[i].ID);
		printf("(Adresse: %d, ", file->tab[i].Bloc);
		printf("Deplacement: %d)\n", file->tab[i].Offset);
	}
}

// supprimer un couple (ID, adresse) de l'index (entrée)
void suppressionEntre(FileIndex *file, int cle){
	int i=rechercheCle(file,cle);
	if(i==-1){
		color(3);
		printf("Le contact n'existe pas.\n");
		resetClor();
		return;
	}
	for(int j=i; j < file->taille-1; j++){
		file->tab[i] = file->tab[i+1];
	}
	file->taille--;
}


// Ajouter un couple (ID, adresse) à l'index (entrée)
void insertionEntre(FileIndex *file, int cle, int adr_B, int adr_E){
	if(rechercheCle(file,cle)!=-1){		//si l'ID existe déjà
		return;
	}
	if(file->taille < MAX_INDEX_TAILLE){ 		//si l'index n'est pas plein
		file->tab[file->taille].ID = cle;
		file->tab[file->taille].Bloc = adr_B;
		file->tab[file->taille].Offset = adr_E;
		file->taille++;
		return;
	}
    color(1);
	printf("\tL'index est plein!\n");
	resetClor();
}


void trierIndex(FileIndex *file){	//Bubble sort
	int i=0, j;
	TableIndex tmp;
	bool estOrdonne = false;
	while(!estOrdonne && i < file->taille){
		estOrdonne = true;
		for(j = 0; j < file->taille-i-1; j++){
			if(file->tab[j+1].ID < file->tab[j].ID){
				tmp = file->tab[j];
				file->tab[j] = file->tab[j+1];
				file->tab[j+1] = tmp;
				estOrdonne = false;
			}
		}
		i++;
	}
}

// helper function
void Lire_Contact_Fichier(LVC* fileLVC,buffer buf,int adr_B, int adr_E,char *contact){
	if(!contact) return;
	memset(contact, '\0', 345);
	int i=0;
	while(buf.contact_Tab[adr_E] != '#'	&&	adr_E<B){	//lecture de la taille du contact
		contact[i] = buf.contact_Tab[adr_E];
		adr_E++;
		i++;
	}
	if(adr_E==B && buf.contact_Tab[adr_E-1] != '#'){		//s'il y a un chevauchement
		adr_E--;
		lireBloc(fileLVC, adr_B+1, &buf);
		int k=0;
		while(buf.contact_Tab[k] != '#'){
			contact[i] = buf.contact_Tab[k];
			k++;
			adr_E++;
			i++;
		}
	}
}

void remplirIndex(FileIndex *file, LVC *fileLVC,int nbreGenere,int nbreInsereIDs,int insereIDs[]){
	file->taille = nbreGenere + nbreInsereIDs;	//taille de table d'index

	int i, j=0;
	int *adr_B=malloc(sizeof(int));
	int *adr_E=malloc(sizeof(int));
	buffer buf;

	// remplissage des @ du contacts générés aléatoirement
	for ( i = 1; i <= nbreGenere; i++)	//i représente l'ID
	{
		recherche(fileLVC,i,adr_B,adr_E);
		if (*adr_B==-1 && *adr_E==-1)
		{
			continue;
		}
		
		insertionEntre(file, i, *adr_B, *adr_E);
	}
	fermer(fileLVC);
	// réouverture du fichier pour la lecture (ça ne marche pas si on ne le fait pas ㄟ( ▔, ▔ )ㄏ)
	fileLVC = ouvrir(NOM_FICHIER, 'r');
	// remplissage des @ des contacts insérés
	for ( i = 0; i < nbreInsereIDs; i++)
	{
		recherche(fileLVC,insereIDs[i],adr_B,adr_E);
		if (*adr_B==-1 && *adr_E==-1)
		{
			continue;
		}
		insertionEntre(file, insereIDs[i], *adr_B, *adr_E);
	}
	trierIndex(file);
	free(adr_B);
	free(adr_E);
}

void afficherContact(FileIndex *file, int i){
	buffer buf;
	LVC *fileLVC = ouvrir(NOM_FICHIER, 'r');
	if (fileLVC == NULL)
	{
		color(1);
		printf("\tErreur d'ouverture du fichier %s\n", NOM_FICHIER);
		resetClor();
		return;
	}
	
	lireBloc(fileLVC,file->tab[i].Bloc, &buf);
	char *contact = malloc(345 * sizeof(char)); 	//345 pour la taille maximale d'un contact (l'observation est de taille 250 au maximum)
	Lire_Contact_Fichier(fileLVC,buf,file->tab[i].Bloc,file->tab[i].Offset,contact);

	char *savePtr=NULL;

	printf("ID: %d\n", file->tab[i].ID);
	printf("Nom: %s\n", strtok_s(contact, "|", &savePtr));
	printf("Tel: %s\n", strtok_s(NULL, "|", &savePtr));
	printf("Email: %s\n", strtok_s(NULL, "|", &savePtr));
	printf("Observation: %s\n", strtok_s(NULL, "|", &savePtr));
	fermer(fileLVC);
}

void afficherContactParCle(FileIndex *file, int cle){
	int i = rechercheCle(file, cle);
	if(i == -1){
		color(3);
		printf("Le contact n'existe pas.\n");
		resetClor();
		return;
	}
	afficherContact(file, i);
}
