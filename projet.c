#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct liste {                   // Cette structure représente la liste des positions des mots dans le texte par rapport aux phrases  <------------
   int position;                                                                                                                           //     |                                           
   struct liste *suivant;                                                                                                                  //     |                                 
};                                                                                                                                         //     |
//                                                                                                                                         //     |
struct noeud {                                                                                                                             //     |
   char *mot;                   // chaine de caractères contenant le mot                                                                   //     |
   struct liste *positions;     // liste chainée contenant les positions du mot dans le fichier ---------------------------------------------------
   struct noeud *gauche;        // pointeur vers le fils gauche de l'arbre
   struct noeud *droite;        // pointeur vers le fils droit de l'arbre
};

//////////////////////////////////////////  PARTIE A : INDEXATION DU FICHIER  //////////////////////////////////////

void inserer_mot(struct noeud **arbre, char *mot, int position){   // CETTE FONCTION PERMET D'INSERER LES MOTS RECUPERES DANS NOTRE ARBRE DE RECHERCHE

    if (*arbre == NULL){      // l'arbre est vide, on crée un nouveau noeud pour le mot
        
        *arbre = malloc(sizeof(struct noeud));              // allocation de la memoire pour la création d'un maillon
        (*arbre)->mot= strdup(mot);                         // strdup sert a dupliquer le mot passe en paramètre dans le champ (*arbre)->mot
        (*arbre)->positions = NULL;                         // INITIALISER tous les champs restants (Le préparer pour l'appel)
        (*arbre)->gauche = NULL;                            // Pareil pour le gauche 
        (*arbre)->droite = NULL;                            // Pareil pour le droit
        ajouter_position(*arbre, position);                 // On fait appel à la fonction "ajouter_position" pour remplir la liste des positions du mot 

    } else {                  // Sinon l'arbre n'est pas vide alors on insère soit coté gauche soit coté droit

        int comparer = strcmp(mot, (*arbre)->mot);          // la variable comparer permet de comparer grace au code ASCII les mots
        if (comparer < 0){      
            inserer_mot(&(*arbre)->gauche, mot, position);  // le mot est inférieur au mot clé du noeud, on continue à gauche  
        } else if (comparer > 0){
            inserer_mot(&(*arbre)->droite, mot, position);  // le mot est supérieur au mot clé du noeud, on continue à droite   
        } else {
            struct liste *P = (*arbre)->positions;          // Le pointeur 'P' va parcourir toute la liste des positions du mot pour ajouter la position
            while (P != NULL){
                if (P->position == position){               // si la position du mot est déjà présente dans la liste, ne fait rien 
                    return;                                 //  (C'est à dire éviter les occurences du mot dans la même phrase)                    
                }
                P = P->suivant;                            
            }
            ajouter_position(*arbre, position);             // sinon, ajoute la position à la liste
        }
    }

}

void indexer(struct noeud **arbre, char *nom_fichier){      // CETTE FONCTION EST LA PLUS IMPORTANTE CAR C'EST ELLE QUI VA LIRE LE FICHIER ET SEPARER LES MOTS

    FILE *fichier = fopen(nom_fichier, "r");                          // ouvre le fichier en lecture
    if (fichier == NULL){
        printf("Erreur lors de l'ouverture du fichier\n");
        return;
    }
    char copie[30];                                                   // Cette variable permettra de faire la copie
    char cop;                                                         // Cette variable permettra de lire le fichier caractère par caractère
    int pos=1, i=0;                                                   // La variable pos permettra de savoir la position du mot dans le texte par rapport aux phrases
    while((cop=fgetc(fichier))!=EOF){                                 // Tant que le fichier est encore ouvert
        
        if(cop!='.' && cop!=' ' && cop!=',' && cop!=10 && cop!='!' && cop!='?' && cop!=':' ){  // Si le caractère lu n'est pas un délimiteur de phrase alors
            copie[i]=cop;                                             // on remplit notre chaine de caractère pour la préparer à l'insertion 
            i++;
        }
        else if(cop==' '){                                            // Si le caractère lu est un espace ou une virgule alors on a fini le mot on doit  <---------------
            copie[i]=0;                                            // mettre la case qui succède le dernier caratère à NULL et on l'insère dans l'arbre avec l'appel |                                                                                     |
            i=0;                                                                                                                                                     // |
            inserer_mot(arbre,copie,pos);                                                                                                                            // |
        }                                                                                                                                                            // |
        else if(cop== '.' || cop == '?' || cop == '!'){               // Si le caractère lu est un point alors il s'agit d'une nouvelle phrase                          | 
            copie[i]=0;                                            //  on doit donc incrémenter la position et on fait la même chose faite ici -----------------------
            i=0;                                                                                                                                                     
            inserer_mot(arbre,copie,pos);                             
            pos++;
        }
        
    }
    fclose(fichier);                                                  // ferme le fichier
}

void ajouter_position(struct noeud *noeud, int position){ // CETTE FONCTION PERMET D'AJOUTER LA POSITION RECUE EN PARAMETRE A LA LISTE CHAINEE RESPECTIVE A CHAQUE MOT

    struct liste *new = malloc(sizeof(struct liste));    // crée un nouvel élément de liste pour stocker la position
    new->position = position;
    new->suivant = NULL;
    if (noeud->positions == NULL){                       // si la liste est vide, on ajoute le nouvel élément en tête de liste
        noeud->positions = new;             
        return;
    }
    struct liste *P = noeud->positions;                  // sinon, parcours la liste jusqu'à trouver le dernier élément et ajoute le nouvel élément à la fin de la liste
    while (P->suivant != NULL){
        P = P->suivant;
    }
    P->suivant = new;

}

void afficher_arbre(struct noeud *arbre){                   // CETTE PROCEDURE PERMET D'AFFICHER NOTRE ARBRE UNE FOIS REMPLI  (PARCOURS INFIXE POUR AFFICHER L'ARBRE ALPHABETIQUEMENT)

    if (arbre == NULL){                                     // si l'arbre est vide, ne fait rien
        return;
    }
    afficher_arbre(arbre->gauche);                          // affiche d'abord les mots du sous-arbre gauche
    printf("Mot: %s avec Positions: ", arbre->mot);         // affiche le mot du noeud courant
    struct liste *P = arbre->positions;                     // affiche la liste des positions
    afficher_liste(P);                                      // Fonction qui affiche la liste des positions
    printf("\n");
    afficher_arbre(arbre->droite);                          // affiche ensuite les mots du sous-arbre droit
}

//////////////////////////////////////////  PARTIE B : RECHERCHE DE LA PHRASE  //////////////////////////////////////

struct noeud *rechercher_mot(struct noeud *arbre, char *mot){ // CETTE FONCTION va nous permettre de rechercher le mot dans l'arbre

    if (arbre == NULL){                              // si l'arbre est vide, retourne NULL
        return NULL;
    }

    int comparaison = strcmp(mot, arbre->mot);       // sinon, compare le mot avec celui du noeud courant
    if (comparaison < 0){                            // si le mot est plus petit, recherche le mot dans le sous-arbre gauche
        return rechercher_mot(arbre->gauche, mot);
    }else if (comparaison > 0){                      // si le mot est plus grand, recherche le mot dans le sous-arbre droit
        return rechercher_mot(arbre->droite, mot);
    }else{                                           // sinon, le mot a été trouvé, retourne le noeud correspondant
        return arbre;
    }
}

struct liste *intersection(struct liste *liste1, struct liste *liste2){   // CETTE FONCTION Va nous permettre de faire l'intersection entre deux listes chainées de positions

    struct liste *resultat = NULL;                                    // INISTIALISER LA liste chainée "resultat" pour stocker les positions communes
    struct liste *P = liste1;                                         // element courant de la première liste
    while (P != NULL){
        struct liste *Q = liste2;                                     // element courant de la seconde liste
        while (Q != NULL){
            if (P->position == Q->position){                          // si les positions sont égales, ajoute la position au résultat (INTERSECTION)
                ajouter_position_resultat(&resultat, P->position);    // L'ajout se fait via la fonction "ajouter_position_resultat" 
            }
            Q = Q->suivant;                                           // On avance dans la liste 2 pour recomparer avec l'element de la liste 1 
        }
        P = P->suivant;                                               // Une fois l'itération fini alors on avance dans la liste 1 pour tous recomparer avec les éléments de la liste 2
    }
    return resultat;                                                  // La liste resultat est donc la liste d'intersection complète
}

void afficher_liste(struct liste *liste){       // CETTE FONCTION PERMETTRA D'AFFICHER UNE LISTE CHAINEE UTILISEE ULTERIEUREMENT POUR AFFICHER LA LISTE RESULTAT ET LES LISTES DES POSITIONS

    if (liste == NULL){         // si la liste est vide, ne fait rien
        return;
    }
    struct liste *P = liste;    // parcours la liste et affiche chaque position
    while (P != NULL){
        printf("%d ", P->position);
        P = P->suivant;
    }
    
}

void ajouter_position_resultat(struct liste **resultat, int position){  // CETTE FONCTION PERMET D'AJOUTER LES POSITIONS TROUVES PRECEDEMMENT PAR L'INTERSECTION DANS LA LISTE RESULTAT

    struct liste *nouveau = malloc(sizeof(struct liste));       // Crée un maillon pour stocker la position
    nouveau->position = position;
    nouveau->suivant = NULL;
    if (*resultat == NULL){                                     // Si la liste est vide alors on insère d'abord au début
        *resultat = nouveau;
    }
    else {                                                      // Sinon on parcours toute la liste et on insère à la fin 
    struct liste *P = *resultat;
    while(P->suivant != NULL){
        P = P->suivant;
    }
    P->suivant = nouveau;
    }

}

void rechercher_phrase(struct noeud *arbre, char *phrase){   // CETTE FONCTION NOUS PERMETTRA DE FAIRE LA RECHERCHE DE LA PHRASE

    char *mot;                                                                  // mot courant de la phrase
    struct liste *positions;                                                    // liste des positions du mot courant dans l'arbre
    struct noeud *node;                                                         // maillon du mot à trouver dans l'arbre
    struct liste *resultat = NULL;                                              // liste des positions de la phrase dans le fichier
    mot = strtok(phrase, " ");                                                  // sépare la chaine de caractères en mots en utilisant l'espace comme délimiteur
    while (mot != NULL){                                                        // parcours tous les mots de la phrase
        node = rechercher_mot(arbre, mot);                                      // récupère le noeud du mot
        if(node==NULL){                                                         // Si node == NULL cela veut dire que le mot n'existe pas
            printf("Le mot '%s' n'a pas été trouvé dans le fichier.\n", mot);
            return;                                                             // arrète la recherche de la phrase
        }                                                                      
        positions = node->positions;                                            // récupère la liste des positions de ce mot
        
        if (resultat == NULL){                                                  // si c'est le premier mot de la phrase
            resultat = positions;                                               // le résultat est la liste des positions du premier mot
        } else {                                                                // si ce n'est pas le premier mot de la phrase
            resultat = intersection(resultat, positions);                       // calcule l'intersection entre les positions précédentes et les positions du mot courant
            if (resultat == NULL){                                              // si l'intersection est vide, la phrase n'existe pas dans le fichier
                printf("La phrase n'a pas été trouvée dans le fichier.\n");
                return;                                                         // arrète la recherche de la phrase
            }
        }
        mot = strtok(NULL, " ");                                                // passe au mot suivant
    }
    if(resultat->suivant==NULL){
        printf("La phrase a été trouvée dans le fichier à la position : ");
    }else{
        printf("La phrase a été trouvée dans le fichier aux positions : ");
    }                                                                           // Si on arrive jusque la cela veut dire que la phrase existe
    
    afficher_liste(resultat);                                                   // affiche les positions de la phrase dans le fichier

}

////////////////////////////////////////// MAIN //////////////////////////////////////

int main(void){

    struct noeud *arbre = NULL;                         // On initialise l'arbre à NULL
    indexer(&arbre, "test.txt");                        // On indexe d'abord l'arbre en lisant le fichier
    afficher_arbre(arbre);                              // L'arbre est rempli il ne manque plus qu'à l'afficher pour voir où se trouvent tous les mots de cet arbre
    
    char phrase[100];
    printf("\n************ Recherche de la phrase ************\n");
    printf("Entrez une phrase : ");                     // Nous allons commencer la recherche
    
    fgets(phrase, sizeof(phrase), stdin);               // Pour cela l'utilisateur doit entrer une chaine de caractères
                                                                  
    phrase[strcspn(phrase, "\n")] = 0;                  // Puis mettra un NULL a la case qui la succède pour éviter toute invalidation d'accès mémoire
    printf("Vous avez entré : %s\n", phrase);

    rechercher_phrase(arbre,phrase);                    // La recherche se fait et l'affichage se fait aussi dans la fonction elle même
    return 0;

}