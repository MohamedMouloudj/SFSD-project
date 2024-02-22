#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "texteColor.c"
#include "aleatoire.c"
#include "flvc.c"
#include "Table_indx.c"

char *nom_Fichier="Contacts.bin";
int N;
int N_a_Insere;

int main(){

    boldColor(7);
    printf("Ins%crez le nombre de contacts %c g%cn%crer: ",130,133,130,130);
    resetClor();
    scanf("%d",&N);
    srand(time(NULL));
    
    //creation du fichier avec "N" contacts aleatoires
    LVC *file=malloc(sizeof(LVC));
    creation_Fichier(file,N);

    FileIndex *fileIndex=ouvrirT('w');
    if(fileIndex==NULL){
        printf("Erreur d'ouverture du fichier d'index\n");
    }

    color(5);
    printf("\nApr%cs la cr%ceation du fichier:\n",138,130);
    resetClor();
    afficher_Entete(file);

    boldColor(7);
    printf("Ins%crez le nombre de contacts %c ins%crer: ",130,133,130);
    resetClor();
    scanf("%d",&N_a_Insere);

    //insertion des "N_a_Insere" contacts fournis par l'utilisateur
    file=ouvrir(nom_Fichier,'r');
    int *prev_ID=malloc(sizeof(int));
    *prev_ID=N;
    int prev_IDs[N_a_Insere];   //pour stocker les IDs des contacts inseres , pour le remplissage de l'index
    for (int i = 0; i < N_a_Insere; i++)
    {
        inserer(file,NULL,false,N,prev_ID);
        prev_IDs[i]=*prev_ID;
    }

    remplirIndex(fileIndex,file,N,N_a_Insere,prev_IDs);

    fermer(file);    


    boldColor(2);
    printf("\tAjout%cs avec succ%cs\n",130,130);
    resetClor();

    color(5);
    printf("\nApr%cs l'insertion des contacts:\n",138);
    resetClor();
    afficher_Entete(file);

    boldColor(7);
    printf("Ins%crez l'ID du contact recherch%c ",130,130);
    printf("(Il faut respecter les valeurs des IDs d%cj%c ins%cr%cs et le nombre du contacts g%cn%cr%cs): ",130,133,130,130,130,130,130,130);
    resetClor();
    
    int id;
    scanf("%d",&id);
    int *adresse_bloc=malloc(sizeof(int));
    int *deplacement=malloc(sizeof(int));
    recherche(file,id,adresse_bloc,deplacement);
    boldColor(4);
    printf("Recherche simple sans table d'index:\n");
    resetClor();
    printf("Adresse du bloc: %d, Deplacement: %d\n",*adresse_bloc,*deplacement);

    boldColor(4);
    printf("Recherche avec table d'index:\n");
    resetClor();
    int index=rechercheCle(fileIndex,id);
    if(index!=-1) printf("Adresse du bloc: %d, Deplacement: %d    (Table d'idexe)\n",fileIndex->tab[index].Bloc,fileIndex->tab[index].Offset);
    else printf("ID invalide\n");
    boldColor(7);
    printf("\nIns%crez le nombre de contacts %c supprimer: ",130,133);
    resetClor();
    int nSup;
    do
    {
        color(3);
        printf("Veuillez ins%crer un nombre entre 0 et %d\n",130,N+N_a_Insere);
        resetClor();
        scanf("%d",&nSup);
    } while (nSup<0 || nSup>N+N_a_Insere);
    
    
    
    for (int i = 0; i < nSup; i++)
    {       
        int supId;
        boldColor(7);
        printf("Ins%crez l'ID du contacts %c supprimer: ",130,133);
        resetClor();

        scanf("%d",&supId);
        char reponse;
        while (!suppression(file,supId))
        {
            color(3);
            printf("ID invalide!\nVoulez-vous ressaisir un valide ID? ('y' pour oui)\n");
            resetClor();

            scanf(" %c",&reponse);getchar();
            if (reponse!='y')
            {
                break;
            }
            
            boldColor(7);
            printf("Ins%crez l'ID du contacts %c supprimer: ",133);
            resetClor();
            scanf("%d",&supId);getchar();
        }
        suppressionEntre(fileIndex,supId);
    }
    
    color(5);
    printf("\nApr%cs la suppression des contacts:\n",138,130);
    resetClor();
    afficher_Entete(file);

    afficher_fichier(file);
    
    bool quitter=false;
    char reponse;
    printf("Voulez-vous quitter? ('y' pour oui)\n");
    while(!quitter)
    {
        scanf(" %c",&reponse);getchar();
        if(reponse=='y'){
            quitter=true;
        }
    }
    
    fermerT(fileIndex);

    return 0;
}