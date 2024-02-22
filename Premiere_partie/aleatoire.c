#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//type=0 pour les numéros, type=1 pour les chaines, type=2 pour les booléens, type=3 pour les emails
void Chaine_Alea(int type,char* chaineOriginal){

    char* alpha = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"; //Une chaine qui contient tous les caract�res possibles
    char* num_Rand = "0123456789";//Une chaine contient tous le num�ros
    char* bool_Rand = "01";//Une chane pour Vrai ou Faux

    if (type == 0) {//Pour les numéros aléatoires

        for (int i = 0; i < 10; i++)
        {
            chaineOriginal[i] = num_Rand[rand() % 10]; //Choisir un numero aleatoirement
        }
        chaineOriginal[10] = '|'; //Ajouter un separateur
        chaineOriginal[11] = '\0'; //Un caractere vide pour determiner la fin de la chaineOriginal de caracteres
    }else
    if (type == 1) {//Pour les chaineOriginals aleatoires

        for (int i = 0; i < 30; i++)
        {
            chaineOriginal[i] = alpha[rand() % 52]; //Choisir un caract�re al�atoirement
        }
        chaineOriginal[30] = '|'; //Ajouter un s�p�rateur
        chaineOriginal[ 31] = '\0'; //Un caract�re vide pour d�terminer la fin de la chaineOriginal de caract�res
    }else
    if (type == 2) {//Pour un boolean al�atoire

        chaineOriginal[0] = bool_Rand[rand() % 2]; //Choisir une valeur al�atoirement
        chaineOriginal[1] = '|'; //Ajouter un s�p�rateur
        chaineOriginal[2] = '\0'; //Un caract�re vide pour d�terminer la fin de la chaineOriginal de caract�res
    }else
    if (type == 3) {//Pour un emaile al�atoires

        for (int i = 0; i < 20; i++)
		{
			chaineOriginal[i] = alpha[rand() % 52]; //Choisir un caract�re al�atoirement
		}
        strcat(chaineOriginal,"@gmail.com");
        chaineOriginal[30] = '|'; //Ajouter un s�p�rateur
		chaineOriginal[31] = '\0'; //Un caract�re vide pour d�terminer la fin de la chaineOriginal de caract�res
    }
    else {
        printf("\n\tCe mode n'%cxiste pas!", 130);
        return;
    }

}

char* observation_Alea() {//Puor générer une observation al�atoire

	char* alpha = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.";

    int observation_taille = (rand() % 200) + 48; //Choisir une taille aléatoire entre 48 et 248
	char* str = (char*)malloc(sizeof(char) * (observation_taille + 2));
	for (int i = 0; i < observation_taille; i++)
	{
		str[i] = alpha[rand() % 64]; 
	}
	str[observation_taille] = '|'; //Ajouter un séparateur (taille max = 249)
	str[observation_taille+1] = '\0'; //Un caractère vide pour déterminer la fin de la chaineOriginal de caractères (taille max = 250)
	return str;
}