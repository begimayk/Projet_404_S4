/* ------------------------------------------------------------------------
-- paquetage analyse_lexicale
--
-- analyse 
-- J-lexicale d'une expression arithmetique
--P. Peyrin, G. Serasset (version initiale) : janvier 1999
-- P.Habraken - 18/01/2010
--
-- 10 juin 2006 - PH :
-- remplacement record a champs variants par record "plat"
-- remplacement traitement iteratif par automate
--
-- Aout 2016 - LM : version C
------------------------------------------------------------------------ */

#include <string.h>
#include <stdlib.h> 
#include <stdio.h> 

#include "lecture_caracteres.h"
#include "analyse_lexicale.h"

#define NB_MOTCLE 2
char motCle[NB_MOTCLE][20] = {"lire", "ecrire"};
   /* --------------------------------------------------------------------- */

  void est_motCle(Lexeme *lexeme_en_cours){
    for(int i = 0; i<NB_MOTCLE; i++){
      if (strcmp(lexeme_en_cours->chaine,motCle[i]) == 0){
        switch(i){
          case 0:
            lexeme_en_cours->nature = LIRE;
            break;
          case 1:
            lexeme_en_cours->nature = ECRIRE;
            break;
          default:
            break;
        }
      }
    }
  }
/* Les variables et fonctions locales au module */

   Lexeme lexeme_en_cours ;	/* le lexeme courant */

   void ajouter_caractere (char *s, char c);
   Nature_Caractere nature_caractere (char c);
   int est_separateur(char c ) ;
   int est_chiffre(char c ) ;
   int est_symbole(char c ) ;
   //int est_commande(char c) ;
   int est_virgule(char c) ;
   int est_lettre_aff(char c);
   void reconnaitre_lexeme();
   Nature_Lexeme Chaine_vers_Nature (Lexeme l);

   /* --------------------------------------------------------------------- */

   void demarrer(char *nom_fichier) {
      demarrer_car(nom_fichier);
      avancer();
   }

   /* --------------------------------------------------------------------- */

   void avancer() {
      reconnaitre_lexeme();
   } 

   /* --------------------------------------------------------------------- */

   Lexeme lexeme_courant() {
      return lexeme_en_cours;
   } 

   /* --------------------------------------------------------------------- */

   int fin_de_sequence() {
      return lexeme_en_cours.nature == FIN_SEQUENCE;
   }

   /* --------------------------------------------------------------------- */

   void arreter() {
      arreter_car() ;
   }

   /* --------------------------------------------------------------------- */

   /* --------------------------------------------------------------------- */


   // reconnaissance d'un nouveau lexeme
   // etat initial : le caractere courant est soit separateur 
   //                soit le 1er caractere d'un lexeme
   // etat final : 
   //       - un nouveau lexeme est reconnu dans lexeme_en_cours
   //       - le caractere courant est soit la fin de fichier, 
   //		soit un separateur,  soit le 1er caractere d'un lexeme

   void reconnaitre_lexeme() {
      typedef enum {E_INIT, E_IDF, E_ENTIER, E_FIN} Etat_Automate ;
      Etat_Automate etat=E_INIT;

     // on commence par lire et ignorer les separateurs
     while (est_separateur(caractere_courant())) {
        	avancer_car() ;
     } ;
     lexeme_en_cours.chaine[0] = '\0' ;

     // on utilise ensuite un automate pour reconnaitre et construire le prochain lexeme

    while (etat != E_FIN) {

	    switch (etat) {
      
		  case E_INIT: // etat initial
			switch(nature_caractere(caractere_courant())) {
				case C_FIN_SEQUENCE: 
             		lexeme_en_cours.nature = FIN_SEQUENCE;
                	etat = E_FIN;
				break ;
        case LETTRE:
                lexeme_en_cours.ligne = numero_ligne();
                lexeme_en_cours.colonne = numero_colonne();
		     		    ajouter_caractere (lexeme_en_cours.chaine, caractere_courant()) ;
                lexeme_en_cours.valeur = caractere_courant() - '0';
                lexeme_en_cours.nature =IDF;
                etat = E_IDF;
                avancer_car() ;
        break;
				case CHIFFRE: 
		     		lexeme_en_cours.nature = ENTIER;
               lexeme_en_cours.ligne = numero_ligne();
               lexeme_en_cours.colonne = numero_colonne();
		     		ajouter_caractere (lexeme_en_cours.chaine, caractere_courant()) ;
               lexeme_en_cours.valeur = caractere_courant() - '0';
               etat = E_ENTIER;
               avancer_car() ;
					break;

				case SYMBOLE:
		       		lexeme_en_cours.ligne = numero_ligne();
               	lexeme_en_cours.colonne = numero_colonne();
		       		ajouter_caractere (lexeme_en_cours.chaine, caractere_courant()) ;
               		switch (caractere_courant()) {
               		case '+':
               			lexeme_en_cours.nature = PLUS;
               			etat = E_FIN;
			   			break;
               		case '-':
               			lexeme_en_cours.nature = MOINS;
               			etat = E_FIN;
			   			break;
               		case '*':
               			lexeme_en_cours.nature = MUL;
               			etat = E_FIN;
			   			break;
                  case '/':
               			lexeme_en_cours.nature = DIV;
               			etat = E_FIN;
			   			break;
                  case '(':
               			lexeme_en_cours.nature = PARO;
               			etat = E_FIN;
			   			break;
                  case ')':
               			lexeme_en_cours.nature = PARF;
               			etat = E_FIN;
			   			break;
                  case '=':
                    lexeme_en_cours.nature = AFF;
               			etat = E_FIN;
              break;
                  case ';':
                    lexeme_en_cours.nature = SEPAFF;
               			etat = E_FIN;
              break; 
                 default:
						      printf("Erreur_Lexicale à la ligne %d et colonne %d\n",lexeme_en_cours.ligne,lexeme_en_cours.colonne);
                        etat = E_FIN;
				 	      };
		   			avancer_car() ;
				break;
				default:
		           printf("Erreur_Lexicale à la ligne %d et colonne %d\n",lexeme_en_cours.ligne,lexeme_en_cours.colonne) ;
                 avancer_car ();
                 etat = E_FIN;
		} ;
		break ;
    case E_IDF: // reconnaitre un identificateur
      switch(nature_caractere(caractere_courant())) {
        case LETTRE:
        case CHIFFRE:
          lexeme_en_cours.ligne = numero_ligne();
          lexeme_en_cours.colonne = numero_colonne();
          ajouter_caractere (lexeme_en_cours.chaine, caractere_courant()) ;
          lexeme_en_cours.valeur = caractere_courant() - '0';
          avancer_car ();
          break;
        default:
          est_motCle(&lexeme_en_cours);
          etat = E_FIN;
      }
    break;
		case E_ENTIER:  // reconnaissance d'un entier
			switch(nature_caractere(caractere_courant())) {
          case CHIFFRE:
               lexeme_en_cours.ligne = numero_ligne();
               lexeme_en_cours.colonne = numero_colonne();
		  			ajouter_caractere (lexeme_en_cours.chaine, caractere_courant()) ;
               lexeme_en_cours.valeur = lexeme_en_cours.valeur * 10 + caractere_courant() - '0';
               etat = E_ENTIER;
               avancer_car ();
					break ;
            case LETTRE:
                printf("Erreur : nom de variable invalide  à la ligne %d et colonne %d!\n",numero_ligne(),numero_colonne());
                etat = E_FIN;
                break;
				default:
               etat = E_FIN;
          	} ;
      break;
	   case E_FIN:  // etat final
		break ;
	  } ; // fin du switch(etat)
	} ; // fin du while (etat != fin)
   }

   /* --------------------------------------------------------------------- */

   // cette fonction ajoute le caractere c a la fin de la chaine s 
   // (la chaine s est donc modifiee)
 
   void ajouter_caractere (char *s, char c) {
	int l ;
	
	l = strlen(s) ;
	s[l] = c ;
	s[l+1] = '\0' ;
   } ;

   /* --------------------------------------------------------------------- */

   Nature_Caractere nature_caractere (char c) {
	   if (fin_de_sequence_car(c)) return C_FIN_SEQUENCE;
	   if (est_chiffre(c)) return CHIFFRE;
     if (est_virgule(c)) return VIRGULE;
     //if (est_commande(c)) return COMMANDE;
	   if (est_symbole(c)) return SYMBOLE;
     if (est_lettre_aff(c)) return LETTRE;
	   return ERREUR_CAR;
   }
   /* --------------------------------------------------------------------- */

   // vaut vrai ssi c designe un caractere separateur
   int est_separateur(char c) { 
      return c == ' ' || c == '\t' || c == '\n' ;
   }

   /* --------------------------------------------------------------------- */
   
/*    // vaut vrai ssi c designe une affectation
   int est_affectation(char c) {
      return (c == '=');
   } */

   /* --------------------------------------------------------------------- */

   // vaut vrai ssi c designe un caractere chiffre
   int est_chiffre(char c) {
      return (c >= '0' && c <= '9') ;
   }

   /* --------------------------------------------------------------------- */
   /* --------------------------------------------------------------------- */
   
   int est_lettre_aff (char c){
     //vaut vrai si c est une lettre de l'alphabet : maj ou min 
      return (c >= 'a' && c <= 'z') || (c >='A'  && c <='Z' ) ;
   }

   /* --------------------------------------------------------------------- */
   int est_virgule(char c){
      return c == '.' || c == ',';
   }

   /* --------------------------------------------------------------------- */

   // vaut vrai ssi c designe un caractere correspondant a un symbole arithmetique
   int est_symbole(char c)  {
      switch (c) {
      case '+':  
      case '-':  
      case '*':
      case '/':
      case '(':
      case ')':
      case '=':
      case ';':
            return 1;
      default:
            return 0;
      } 
   }

   /* --------------------------------------------------------------------- */

   // renvoie la chaine de caracteres correspondant a la nature du lexeme
   char *Nature_vers_Chaine (Nature_Lexeme nature) {
	switch (nature) {
		case ENTIER: return "ENTIER" ;
		case PLUS: return "PLUS" ;
    case MOINS: return "MOINS" ;            
    case MUL: return "MUL" ;      
    case DIV: return "DIV";
    case PARO: return "PARO";
    case PARF: return "PARF";
    case REEL: return "REEL";
    case IDF: return "IDENTIFICATEUR";
    case AFF: return "AFF";
    case SEPAFF: return "SEPAFF";
    case LIRE: return "LIRE";
    case ECRIRE: return "ECRIRE";
    case FIN_SEQUENCE: return "FIN_SEQUENCE" ;     
    default: return "ERREUR" ;            
	} ;
   } 
   
   Nature_Lexeme Chaine_vers_Nature (Lexeme l) {
      if (strcmp("plus",l.chaine)==0 || strcmp("PLUS",l.chaine)==0){
         return PLUS;
      }
      else{
         if (strcmp("moins",l.chaine)==0 || strcmp("MOINS",l.chaine)==0){
            return MOINS;
         }
         else{
            if (strcmp("div",l.chaine)==0 || strcmp("DIV",l.chaine)==0){
               return DIV;
            }
            else{
               if (strcmp("mul",l.chaine)==0 || strcmp("MUL",l.chaine)==0){
                  return MUL;
               }
               else{
                  return FIN_SEQUENCE;
               }
            }
         }
      }
   } 
  
   /* --------------------------------------------------------------------- */

   // affiche a l'ecran le lexeme l
   void afficher(Lexeme l) {

      switch (l.nature) {
         case FIN_SEQUENCE: 
            break;
         default: 
            printf("(ligne %d, ", l.ligne);
            printf("colonne %d) : ",l.colonne);
	    printf("[") ;
            printf("nature = %s", Nature_vers_Chaine(l.nature)) ;
            printf(", chaine = %s, ", l.chaine) ;
            switch(l.nature) {
                 case ENTIER:
                      printf("valeur = %f", l.valeur);
                 default:
                      break;
            } ;
	    printf("]") ;
	} ;
   }

   /* --------------------------------------------------------------------- */

