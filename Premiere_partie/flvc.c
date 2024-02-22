#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "flvc.h"
#include "texteColor.h"
#include "aleatoire.h"
#define BUFFER_SIZE 1024
extern char *strtok_s(char *, const char *, char **); //pour utiliser la fonction strtok_s (divise la chaine en sous-chaines appelées "Tokens")

//* Les fonctions élémentaires

// 0: nb_Blocs, 1: adr_Premier_Bloc, 3: nb_Contacts, 4: nb_Contacts_Insere, 5: nb_Contacts_Sup, 6: taill_Fichier
int entete(LVC *file, int carac_Num)
{
	switch (carac_Num)
	{
	case 0:
		return file->tete.nb_Blocs;
		break;
	case 1:
		return file->tete.adr_Premier_Bloc;
		break;
	case 3:
		return file->tete.nb_Contacts;
		break;
	case 4:
		return file->tete.nb_Contacts_Insere;
		break;
	case 5:
		return file->tete.nb_Contacts_Sup;
		break;
	case 6:
		return file->tete.taill_Fichier;
		break;
	default:
		color(1);
		printf("\n\tCe caract%cristique n'%cxiste pas!\n", 130, 130);
		resetClor();
		break;
	}
}

// 0: nb_Blocs, 1: adr_Premier_Bloc, 3: nb_Contacts, 4: nb_Contacts_Insere, 5: nb_Contacts_Sup, 6: taill_Fichier
void affecterEntete(LVC *file, int carac_Num, int val)
{
	switch (carac_Num)
	{
	case 0:
		file->tete.nb_Blocs = val;
		break;
	case 1:
		file->tete.adr_Premier_Bloc = val;
		break;
	case 3:
		file->tete.nb_Contacts = val;
		break;
	case 4:
		file->tete.nb_Contacts_Insere = val;
		break;
	case 5:
		file->tete.nb_Contacts_Sup = val;
		break;
	case 6:
		file->tete.taill_Fichier = val;
		break;
	default:
		color(1);
		printf("\n\tCe caract%cristique n'%cxiste pas!\n", 130, 130);
		resetClor();
		break;
	}
}

void lireBloc(LVC *file, int num_Bloc, buffer *buf)
{
	fseek((file->f), sizeof(Entete) + (num_Bloc - 1) * sizeof(bloc), SEEK_SET);
	fread(buf, sizeof(buffer), 1, file->f);
}

void ecrireBloc(LVC *file, int num_Bloc, buffer *buf)
{
	fseek((file->f), sizeof(Entete) + (num_Bloc - 1) * sizeof(bloc), SEEK_SET);
	fwrite(buf, sizeof(buffer), 1, file->f);
}

LVC *ouvrir(char *nom_Fichier, char mode)
{
	LVC *file = malloc(sizeof(LVC));
	buffer buf;
	if (mode == 'w'){
		file->f = fopen(nom_Fichier, "wb+");
		// Initialisation de la tete
		affecterEntete(file, 0, 1);
		affecterEntete(file, 1, 1);
		affecterEntete(file, 3, 0);
		affecterEntete(file, 4, 0);
		affecterEntete(file, 5, 0);
		affecterEntete(file, 6, sizeof(Entete));
		fwrite(&(file->tete), sizeof(Entete), 1, file->f); // Sauvgarde de l'entete
		buf.suiv = -1;
		sprintf(buf.contact_Tab, "%s", ""); // Initialisation de la chaine de contactes
		ecrireBloc(file, 1, &buf);			  // Insertion du bloc initialisé à la première position de la liste
		affecterEntete(file,6,entete(file,6)+sizeof(buf.suiv)); // Incrémentation de la taille du fichier

	}
	else{
		if (mode == 'r'){
			file->f = fopen(nom_Fichier, "rb+");
			if(file->f==NULL){
				boldColor(1);
				printf("\n\tLe fichier n'existe pas!\n");
				resetClor();
				return NULL;
			}
			fread(&(file->tete), sizeof(Entete), 1, file->f); // Lecture de l'entete
		}else{
			boldColor(1);
			printf("\n\tCe mode d'ouverture n'existe pas!\n");
			resetClor();
			return NULL;
		}
	}
	return file;
}

void fermer(LVC *file)
{
	rewind(file->f);
	fwrite(&(file->tete), sizeof(Entete), 1, file->f); // Sauvgarde de l'entete
	rewind(file->f);
	fclose(file->f);
}

// Pour allouer un bloc dans le fichier
void allouer_Bloc(LVC *file)
{								
	bloc *b = malloc(sizeof(bloc)); // Allouer un espace mémoire pour le buffer

	lireBloc(file, entete(file, 0), b); // Lecture du dernier bloc
	b->suiv = entete(file, 0) + 1;		  // @ du bloc suivant
	ecrireBloc(file, entete(file, 0), b); // Ecriture du bloc modifié

	sprintf(b->contact_Tab, "%s", ""); // Initialisation de la chaine de contactes
	b->suiv = -1;				  //@ du bloc suivant (NULL car c'est le dernier bloc)

	ecrireBloc(file, entete(file, 0) + 1, b);						  // Insertion du bloc initialisé à la première position de la liste
	affecterEntete(file, 0, entete(file, 0) + 1);				  // Incrémentation du nombre de blocs
	affecterEntete(file, 6, entete(file, 6) + sizeof(b->suiv)); // Incrémentation de la taille du fichier
	free(b);
}

//* ///////////////////////////////////////////////////////////////////////////////////


//* préparation du contact dan une chaine pour l'écrire dans le fichier
void creerContact(contact *c, char *contact_A_Insere)
{
	char *contact_Tab = malloc(sizeof(char) * B);							   // Allouer un espace mémoire pour la chaine de contactes
	memset(contact_Tab, '\0', B);											   // Initialisation de la chaine de contactes
	char *taille_Contact = malloc(sizeof(char) * (TAILLE_TAILLE_CONTACT + 2)); // Allouer un espace mémoire pour la taille du contact
	char *est_Efface = malloc(1 + strlen(c->est_Efface));					   // Allouer un espace mémoire pour le champ est_Efface
	char *ID = malloc(1 + strlen(c->ID));									   // Allouer un espace mémoire pour le champ ID
	char *nom = malloc(1 + strlen(c->nom));									   // Allouer un espace mémoire pour le champ nom
	char *tel = malloc(1 + strlen(c->tel));									   // Allouer un espace mémoire pour le champ tel
	char *adresse_Email = malloc(1 + strlen(c->adresse_Email));				   // Allouer un espace mémoire pour le champ adresse_Email
	char *observation = malloc(1 + strlen(c->observation));					   // Allouer un espace mémoire pour le champ observation

	// Initialisation des champs
	strncpy(est_Efface, c->est_Efface, 1 + strlen(c->est_Efface));
	strncpy(ID, c->ID, 1 + strlen(c->ID));
	strncpy(nom, c->nom, 1 + strlen(c->nom));
	strncpy(tel, c->tel, 1 + strlen(c->tel));
	strncpy(adresse_Email, c->adresse_Email, 1 + strlen(c->adresse_Email));
	strncpy(observation, c->observation, 1 + strlen(c->observation));

	// Concaténation des champs
	strcat(contact_Tab, est_Efface);
	strcat(contact_Tab, ID);
	strcat(contact_Tab, nom);
	strcat(contact_Tab, tel);
	strcat(contact_Tab, adresse_Email);
	strcat(contact_Tab, observation);
	// Calcul de la taille du contact
	char pre_taille_Contact[4];		// chaine de taille 4 a cause de la taille maximale d'un contact (sur 3 chiffres) + 1 caractere de fin de chaine
	int taille=strlen(contact_Tab)+4;	// +4 pour le champ: taille de contact
	snprintf(pre_taille_Contact,4, "%d", taille); // Convertir la taille du contact en chaine de caractères de taille 4 (sur 3 chiffres) + 1 caractere de fin de chaine
	strcpy(taille_Contact, pre_taille_Contact);
	strcat(taille_Contact, "|");	 // Ajout du caractère de séparation entre la taille du contact et le reste du contact
	// Concaténation de la taille du contact
	strcat(contact_Tab, taille_Contact);

	// Deallocations de variableds temporaires
	free(taille_Contact);
	free(est_Efface);
	free(ID);
	free(nom);
	free(tel);
	free(adresse_Email);
	free(observation);

	char separateur[] = "#";		 // Caractère de séparation entre les contacts (+ caractère de fin de chaine)
	strcat(contact_Tab, separateur); // Ajout du caractère de fin de contact
	
	strcpy(contact_A_Insere, contact_Tab); // Copie de la chaine de contactes dans la chaine de contact à insérer
	free(contact_Tab);
	contact_Tab=NULL;
}

//* Pour générer un contact aléatoirement
void generer_Contact(contact *c)
{ 
	c->est_Efface[0] = '0';
	c->est_Efface[1] = '|';	 // Ajouter un séparateur
	c->est_Efface[2] = '\0'; // Un caractère vide pour déterminer la fin de la chaine de caractères
	Chaine_Alea(1, c->nom);
	Chaine_Alea(3, c->adresse_Email);
	Chaine_Alea(0, c->tel);
	c->observation = observation_Alea();
}

//* //////////////////////////////////////////////////////////////////////////////////////

//* insertion un contact dans le fichier (soit automatiquement ou manuellement)
void 
inserer(LVC *file, contact *c , bool estAuto, int N, int *current_ID)
{
	char *contact_A_Insere = malloc(sizeof(char) * B); // Allouer un espace mémoire pour le contact à insérer
	memset(contact_A_Insere, '\0', B);	// Initialisation du contact à insérer
	if(estAuto){   
		creerContact(c,contact_A_Insere);					 // Création du contact à insérer
		char *deuxieme_Moitier_Contact;
		char *premier_Moitier_Contact;

		buffer *buf = malloc(sizeof(buffer)); // Allouer un espace mémoire pour le buffer
		lireBloc(file, entete(file, 0), buf);	  // Lecture du dernier bloc

		if (strlen(buf->contact_Tab)<(B-1) && (strlen(buf->contact_Tab) + strlen(contact_A_Insere) <= B)){ // Si le dernier bloc n'est pas plein
			// Si l'espace restant est suffisant pour un contact (pas de chevauchement)
			strcat(buf->contact_Tab, contact_A_Insere); // Inserstion du contact au dernier bloc (remarque: il ya un caractère de fin de chaine dans le conact à insérer)
			ecrireBloc(file, entete(file, 0), buf);		// Ecriture du bloc modifié

		}else if(strlen(buf->contact_Tab) + strlen(contact_A_Insere) > B){				
			// Si l'espace restant n'est pas suffisant pour un contact (Chevauchement)

			int reste = B - strlen(buf->contact_Tab) - 1; // Calculer l'espace restant dans le dernier bloc (-1 pour le caractère de fin de chaine)
			int taille_deuxieme_Moitier_Contact = strlen(contact_A_Insere) - reste; // Calculer la taille de la deuxième moitié du contact

			premier_Moitier_Contact = malloc(sizeof(char) * (reste + 1));						 // Allouer un espace mémoire pour la première moitié du contact
			deuxieme_Moitier_Contact = malloc(sizeof(char) * (taille_deuxieme_Moitier_Contact+1)); // Allouer un espace mémoire pour la deuxième moitié du contact

			strncpy(premier_Moitier_Contact, contact_A_Insere, reste); // Copier la première moitié du contact
			premier_Moitier_Contact[reste] = '\0';				   // Ajout du caractère de fin de chaine
			strcat(buf->contact_Tab, premier_Moitier_Contact); // Concaténation de la première moitié du contact au dernier bloc
			ecrireBloc(file, entete(file, 0), buf);			   // Ecriture du bloc modifié
			free(premier_Moitier_Contact);
			premier_Moitier_Contact=NULL;
			memset(buf->contact_Tab, '\0', B);					// Initialisation de la chaine de contactes
			
			int i = reste;
			int j = 0;
			while (i < strlen(contact_A_Insere))
			{
				deuxieme_Moitier_Contact[j] = contact_A_Insere[i]; // Copier la deuxième moitié du contact
				i++;
				j++;
			}
			deuxieme_Moitier_Contact[taille_deuxieme_Moitier_Contact] = '\0'; // Ajout du caractère de fin de chaine
							
			allouer_Bloc(file);									// Allouer un nouveau bloc pour la deuxi�me moiti� du contact
			lireBloc(file, entete(file,0), buf);				// Lecture du dernier bloc
			strcpy(buf->contact_Tab, deuxieme_Moitier_Contact); // Concaténation de la deuxième moiti� du contact au dernier bloc
			free(deuxieme_Moitier_Contact);
			deuxieme_Moitier_Contact=NULL;
			ecrireBloc(file, entete(file,0), buf);				// Ecriture du bloc modifié
		}else{												// Si le dernier bloc est plein
			
			allouer_Bloc(file);							// Allouer un nouveau bloc
			lireBloc(file, entete(file, 0), buf);		// Lecture du dernier bloc			
			strcpy(buf->contact_Tab, contact_A_Insere); // Concaténation du contact au dernier bloc
			ecrireBloc(file, entete(file, 0), buf);		// Ecriture du bloc modifié
		}
		free(buf);
		buf=NULL;
		affecterEntete(file, 3, entete(file, 3) + 1);						 // Incrémentation du nombre de contacts
		

	}else{

		contact ctn={0};

		boldColor(7);
		printf("Entrez les informations du contact:\n");
		resetClor();

		printf("ID ");
		color(3);
		printf("(doit %ctre grand que %d et diff%crent que l'ID du contact pr%cc%cdent (%d)): ",136,N,130,130,130,*current_ID);
		resetClor();

		rewind(stdin);
		char id[10];
		do{
			fgets(id, 10, stdin);rewind(stdin);
			if(atoi(id)<=N){
				color(1);
				printf("\tErreur: ID doit %ctre grand que %d (nombre de contactes g%cn%cr%cs)!\n",136,N,130,130,130);
				resetClor();
			}
			if (atoi(id)==*current_ID)
			{
				color(1);
				printf("\tErreur: ID doit %ctre diff%crent que %d (ID du contact pr%cc%cdent)!\n",136,130,*current_ID,130,130);
				resetClor();
			}
			
		}while(atoi(id)<=N || atoi(id)==*current_ID);
		*current_ID=atoi(id);

		strncpy(ctn.ID, "00000000", 8-(strlen(id) - 1));
		strncat(ctn.ID, id, strlen(id)-1);
		printf("\tID: '%s'\n",ctn.ID);
		ctn.ID[8] = '|';	 // Ajouter un séparateur
		ctn.ID[9] = '\0'; // Un caractère vide pour déterminer la fin de la chaine de caractères
			
		ctn.est_Efface[0] = '0';
		ctn.est_Efface[1] = '|';	 // Ajouter un séparateur
		ctn.est_Efface[2] = '\0'; // Un caractère vide pour déterminer la fin de la chaine de caractères

		printf("Nom: ");
		fgets(ctn.nom, 32, stdin);rewind(stdin);
		ctn.nom[strlen(ctn.nom)-1] = '|';
		ctn.nom[strlen(ctn.nom)] = '\0';

		printf("Adresse Email: ");
		fgets(ctn.adresse_Email, 32, stdin);rewind(stdin);
		ctn.adresse_Email[strlen(ctn.adresse_Email) - 1] = '|';
		ctn.adresse_Email[strlen(ctn.adresse_Email)] = '\0';

		printf("Tel: ");
		bool isTel=false;
		do{
			fgets(ctn.tel, 12, stdin);rewind(stdin);
			isTel=true;
			int deg=0;
			while(deg<strlen(ctn.tel)-1 && isTel)
			{
				if(!isdigit(ctn.tel[deg])){
					color(1);
					printf("\tErreur: Le num%cro de t%cl%cphone ne doit contenir que des chiffres!\n",130,130,130);
					resetClor();
					isTel=false;
					break;
				}
				deg++;
			}
			
		}while(!isTel);
		ctn.tel[strlen(ctn.tel)-1] = '|';
		ctn.tel[strlen(ctn.tel)] = '\0';

		char *str=malloc(sizeof(char)*249);
		printf("Observation: ");
		memset(str, '\0', 249);
		rewind(stdin);
		fgets(str, 249, stdin);rewind(stdin);
		ctn.observation = malloc(sizeof(char) * (strlen(str)+1));
		strncpy(ctn.observation, str, strlen(str));
		ctn.observation[strlen(str)-1] = '|';
		ctn.observation[strlen(str)] = '\0';
		free(str);
		str=NULL;

		inserer(file, &ctn, true, N,current_ID);
		free(ctn.observation);
		affecterEntete(file, 4, entete(file, 4) + 1);						 // Incrémentation du nombre de contacts insérés
	}
	affecterEntete(file, 6, entete(file, 6) + strlen(contact_A_Insere) + 1); // Incrémentation de la taille du fichier
	free(contact_A_Insere);
	contact_A_Insere=NULL;
}

//* Pour remplir le fichier avec des contacts aléatoires

// Pour calculer le nombre de chiffres dans un nombre (helper function)
int nombre_chiffres(int n)
{
	int cpt = 0;
	while (n > 0)
	{
		cpt++;
		n = n / 10;
	}
	return cpt;
}

void creation_Fichier(LVC *file, int nb_Contacts)
{ 

	int cpt=nombre_chiffres(nb_Contacts);

	if(cpt>8){	// Si le nombre de chiffres de nb_contacts est > 8
		color(1);
		printf("Erreur: ID trop grand!\n");
		printf("Le nombre de contactes doits %ctre un nombre avec le nombre du num%cros <=8!\n", 136, 130);
		resetClor();
		return;
	}

	file=ouvrir(NOM_FICHIER,'w');

	int old_IDs[nb_Contacts];
	for (int i = 0; i < nb_Contacts; i++)
	{
		old_IDs[i] = i + 1; // Remplissage du tableau des old_IDs avec des valeurs de 1 à nb_Contacts distinctes
	}

	contact empty={0}; // Création d'un contact vide pour reinitialiser le contact c
	contact c;
	char *ID = malloc(sizeof(char) * 9); // Allouer un espace mémoire pour l'ID
	int nb=0;

	while(nb<nb_Contacts){	// Génération des ID aléatoires
		c=empty;			 // Reinitialisation du contact c
		memset(ID, '\0', 9);				 // Initialisation de l'ID
		int new_ID;
		do
		{	
			int index = rand() % nb_Contacts;
			new_ID = old_IDs[index]; // Choisir un ID aléatoire
			old_IDs[index] = -1;
		} while (new_ID == -1);
		// Calcul de nombre de chiffres dans new_ID
		cpt = nombre_chiffres(new_ID);
		sprintf(ID, "%d", new_ID);// Convertir l'ID en chaine de caractères
		int k = 0;
		for (; k < 8-cpt; k++)
		{
			c.ID[k] = '0'; // Initialisation de l'ID avec des zéros
		}
		for (int l = k; l < 8; l++)
		{
			c.ID[l] = ID[l-k]; // Ajout du nombre du numéros à l'ID
		}
		c.ID[8] = '|';	 // Ajouter un séparateur
		c.ID[9] = '\0'; // Un caractère vide pour déterminer la fin de la chaine de caractères

		nb++;
		generer_Contact(&c);
		inserer(file, &c,true,0,NULL);
	}

	boldColor(2);
	printf("\n\tLe fichier a %ct%c rempli avec succ%cs!\n", 130, 130, 138);
	resetClor();
	free(ID);
	ID=NULL;
	fermer(file);
}

//* Recherche d'un contact

// helper function, pour lire le champ ID
char *lire_ID(char *contact,buffer* buf,LVC* file){
	char *savePtr=malloc(sizeof(char)*346);		//Pour les contacts
	strncpy(savePtr, contact, 346);
	char *outter_saveptr=NULL;		// Pour la fonction strtok_s
	char *ID_tmp=malloc(sizeof(char)*10);
	char *est_Efface_tmp=malloc(sizeof(char)*5);

	//On a 3 cas pour le champ ID:
	//1- Le champ ID se trouve complètement dans le contact courant 
	//2- Le champ ID se trouve à cheval entre deux blocs
	//3- Le champ ID se trouve complètement dans le bloc suivant (2 cas: l'ID commence au debut du bloc, *(A), ou non )

	est_Efface_tmp=strtok_s(savePtr, "|",&outter_saveptr);	//pour lire le champ est_Efface (utilisé pour le premier cas)
	ID_tmp=strtok_s(NULL, "|",&outter_saveptr);	//pour retourner le champ ID 

	if(est_Efface_tmp && strlen(est_Efface_tmp)==1){

		if(ID_tmp && strlen(ID_tmp)==8){		//cas 1
			return ID_tmp;
		}else if(ID_tmp && strlen(ID_tmp)<8){		//cas 2

			bloc *tmp_Chauv_buffer=malloc(sizeof(bloc));
			char *tmp_Chauv=malloc(sizeof(char)*B);
			memset(tmp_Chauv, '\0', B);
			outter_saveptr=NULL;
			char *inner_savePtr=NULL;

			lireBloc(file,buf->suiv,tmp_Chauv_buffer);
			strcpy(tmp_Chauv, tmp_Chauv_buffer->contact_Tab);
			free(tmp_Chauv_buffer);
			tmp_Chauv_buffer=NULL;

			tmp_Chauv=strtok_s(tmp_Chauv, "#",&outter_saveptr);	//pour retourner le reste de contact qui est dans le bloc suivant
			strcat(ID_tmp,strtok_s(tmp_Chauv, "|",&inner_savePtr));	//pour retourner le reste d'ID qui est trouvé dans le bloc suivant
			return ID_tmp;

		}else{		//cas 3
			
			if(!ID_tmp){
				bloc *tmp_Chauv_buffer=malloc(sizeof(bloc));
				char *tmp_Chauv=malloc(sizeof(char)*B);
				memset(tmp_Chauv, '\0', B);
				outter_saveptr=NULL;
				char *inner_savePtr=NULL;
				
				lireBloc(file,buf->suiv,tmp_Chauv_buffer);
				strcpy(tmp_Chauv,tmp_Chauv_buffer->contact_Tab);
				free(tmp_Chauv_buffer);
				tmp_Chauv_buffer=NULL;

				tmp_Chauv=strtok_s(tmp_Chauv, "#",&outter_saveptr);	//pour retourner le reste de contact qui est dans le bloc suivant
				ID_tmp=strtok_s(tmp_Chauv, "|",&inner_savePtr);	//pour retourner l'ID qui est trouvé dans le bloc suivant
				return ID_tmp;
			}
		}
	}else{
		return NULL;	
	}
}

void recherche(LVC* file,int id, int* adress_bloc, int* deplacement){
	file=ouvrir(NOM_FICHIER,'r');

	if(!file) return;	// Si le fichier n'existe pas

	buffer* buf=malloc(sizeof(buffer));
	lireBloc(file,entete(file,1),buf);

	// Initialisation des addresses
	*adress_bloc=-1;
	*deplacement=-1;
	int i=1;		//Car l'@ du premier bloc est 1
	int j;
	bool isDone=false;

	char *contact=malloc(sizeof(char)*346);		// 344 est la taille maximale d'un contact (sur 3 chiffres) + 1 caractere de fin de chaine + 1 caractere de fin de chaine
	char *tmp_ID=malloc(sizeof(char)*9);
	char *outter_savePtr=NULL;	// Pour la fonction strtok_s
	int taille_lue=0;
	
	do
	{
		j=0; // Pour compter le nombre de caractères dans le bloc (c'est le déplacement)
		contact=strtok_s(buf->contact_Tab, "#", &outter_savePtr);
		while(contact){
			tmp_ID=lire_ID(contact,buf,file);
			if(tmp_ID && atoi(tmp_ID)==id){
				*adress_bloc=i;
				*deplacement=j;
				isDone=true;
				break;
			}
			taille_lue=strlen(contact);
			j+=taille_lue+1;
			contact=strtok_s(NULL, "#\0", &outter_savePtr);
		}
		if(buf->suiv==-1){
			isDone=true;
		}else{
			lireBloc(file,buf->suiv,buf);
			i++;
		}
	}while(!isDone);
	fermer(file);
}

//* Suppression d'un contact (Logique)

bool suppression(LVC *file,int id){
	file=ouvrir(NOM_FICHIER,'r');

	int *adress_bloc=malloc(sizeof(int));
	int *deplacement=malloc(sizeof(int));

	recherche(file,id,adress_bloc,deplacement);

	if(*adress_bloc==-1 && *deplacement==-1){
		boldColor(1);
		printf("\n\tLe contact n'existe pas!\n");
		resetClor();
		return false;
	}

	buffer *buf=malloc(sizeof(buffer));
	char *contact_tab=malloc(sizeof(char)*B);
	memset(contact_tab, '\0', B);

	lireBloc(file,*adress_bloc,buf);	// Lecture du bloc du contact à supprimer
	strcpy(contact_tab,buf->contact_Tab);

	if(contact_tab[*deplacement]=='1'){	// Si le contact est déjà supprimé
		color(1);
		printf("\n\tLe contact est d%cj%c supprim%c!\n",130,130,130);
		resetClor();
		return false;
	}

	contact_tab[*deplacement]='1';	// Suppression logique du contact ("deplacement" est l'indice du champ est_Efface , aussi l'@ du premier caractère du contact)
	strcpy(buf->contact_Tab,contact_tab);

	ecrireBloc(file,*adress_bloc,buf);	// Ecriture du bloc modifié
	affecterEntete(file,5,entete(file,5)+1);	// Incrémentation du nombre de contacts supprimés

	boldColor(2);
	printf("\n\tLe contact a %ct%c supprim%c avec succ%cs!\n",130,130,130,138);
	resetClor();

	free(contact_tab);
	contact_tab=NULL;
	free(buf);
	buf=NULL;
	fermer(file);
	return true;
}


//* Affichage d'entête du fichier

void afficher_Entete(LVC *file){
	file=ouvrir(NOM_FICHIER,'r');

	boldColor(7);
	printf("\n---------------------------\\ ");
	boldColor(6);
	printf("Ent%cte du fichier",136);
	resetClor();
	boldColor(7);
	printf(" /---------------------------\n\n");
	resetClor();

	printf("\tNombre de blocs:\t\t\t%d\n",entete(file,0));
	printf("\tAdresse du premier bloc:\t\t%d\n",entete(file,1));
	printf("\tNombre de contacts:\t\t\t%d\n",entete(file,3));
	printf("\tNombre de contacts ins%cr%cs:\t\t%d\n",130,130,entete(file,4));
	printf("\tNombre de contacts supprim%cs:\t\t%d\n",130,entete(file,5));
	printf("\tTaille du fichier:\t\t\t%d octets\n\n",entete(file,6));

	boldColor(7);
	printf("---------------------------------------------------------------------------\n");
	resetClor();
	fermer(file);
}

//* Affichage de blocs du fichier 
void afficher_fichier(LVC *file){
	file=ouvrir(NOM_FICHIER,'r');
    int nBlocs = entete(file, 0);  // nb_Blocs
    buffer buf;  // buffer pour lire le bloc

    int colorV; // pour l'utuliser dans la fonction boldColor apres le deplacement vers next contact on l'incr
	int oldCorV;
	colorV=(rand() % 7) +1;

	char *tab=malloc(sizeof(char)*B);
	memset(tab, '\0', B);
	char *contact=malloc(sizeof(char)*346);		// 344 est la taille maximale d'un contact (sur 3 chiffres) + 1 caractere de fin de chaine + 1 caractere de fin de chaine
	memset(contact, '\0', 346);
	char *outter_savePtr;	// Pour la fonction strtok_s

	int chauv;	// Pour savoir si le contact est chauve ou non
	int cpt=0;
	int tailleCpt;
	int tailleTab;
	boldColor(7);
    printf("\nLes blocs du fichier:");
    resetClor();
	boldColor(1);
	printf("\t! null <=> ID pr%cc%cdent !\n",130,130);
	resetClor();
    for(int i = 1; i <= nBlocs; i++){ //cette boucle se repete le nbre des blocs

        lireBloc(file, i, &buf); //mettre le bloc i dans le buffer pour qu'on affiche les contacts du bloc a defferents couleurs
		strcpy(tab,buf.contact_Tab);
		tailleTab=strlen(tab);
		
		tailleCpt=0;
		outter_savePtr=NULL;
		contact=strtok_s(tab, "#", &outter_savePtr);

		chauv=0;

		boldColor(7);
		printf("\t-----------------------------------------\n");
		resetClor();
		
		while(contact){
			cpt++;
			tailleCpt+=strlen(contact)+1; // +1 pour le caractère # (séparateur)
			color(colorV);
			printf("\n\t\tContact No %d (ID: %s, %d car)\n",cpt,lire_ID(contact,&buf,file),strlen(contact));
			if (contact[0]=='1')
			{
				color(1);
				printf("\t\tCe contact est supprim%c!\n",130);
				resetClor();
			}
			
			resetClor();

			if(tailleCpt>tailleTab){ // Si le contact est chevauché (monque # à la fin)
				chauv=1;
				cpt--;
				oldCorV=colorV;
			}
			colorV=(rand() % 7)+1 ;
				
			contact=strtok_s(NULL, "#\0", &outter_savePtr);
		}
		if (chauv==1)
		{
			colorV=oldCorV;
		}
		
		color(6);
		printf("\n\t@ bloc suiv: %d\n",buf.suiv);
		resetClor();
		boldColor(7);
		printf("\n\t-----------------------------------------\n\n");
		resetClor();
	}
	free(tab);
	tab=NULL;
	free(contact);
	contact=NULL;
	fermer(file);
	boldColor(1);
	printf("\t! null <=> ID pr%cc%cdent !\n",130,130);
	resetClor();
}