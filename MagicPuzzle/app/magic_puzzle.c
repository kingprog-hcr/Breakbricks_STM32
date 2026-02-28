/*
 * magic_puzzle.c
 *
 *  Created on: Mar 8, 2024
 *      Author: Nirgal
 */
#include "magic_puzzle_hidden_part.h"
#include "magic_puzzle.h"
#include "config.h"
#include "stm32g4_systick.h"
#include <stdio.h>

//------------------------------ETAPE 2------------------------------------
#define SHOW_EXAMPLES	0
void printf_training(void)
{
#if SHOW_EXAMPLES
	/////////////////////////////////////////////////////////////////
	//premiï¿½re partie : observation d'exemples d'utilisation de printf.
	//exï¿½cuter cette partie en step-by-step (over) et observer les valeurs affichï¿½es dans un terminal sï¿½rie.

	printf("Hello World\n We will discover the wonderfull printf function\n");
	printf("%d\n", 42);
	printf("%d\n", 0x42);	//Q1 : qu'affiche cette ligne (sans le '\n' du retour ï¿½ la ligne)

	uint8_t a = '2';	//a vaut le caractï¿½re '2' et non pas le nombre 2
	printf("%d\n",a);
	printf("%c\n",a);
	printf("%x\n",a);    	//Q2 : combien vaut a en hexadï¿½cimal ?

	uint32_t b = 0x12ABCD42;
	printf("%x\n",b);    	//
	printf("%X\n",b);    	//
	printf("%d\n",b);    	//Q3 : qu'affiche cette ligne ?
	printf("%ld\n",b);    	//
	printf("%c\n",b);    	//


	float c = 3.14;
	printf("%f\n",c);    	//Q10

	double d = 3.14159265358979323846;
	float e = (float)d*1000.0;
	printf("%f *1000000 = %f *1000 = %f\n",d*1000000., d*1000., d);
	printf("%f *1000000 = %f *1000 = %f\n",e*1000000., e*1000., e);
	printf("%lf *1000000 = %lf *1000 = %lf\n",e*1000000., e*1000., e);

	printf("%.2lf\n",d);    		//Q13

	printf("Les cours commencent ï¿½ %02d:%02d\n", 8, 0);    	//Q14
	printf("%s ï¿½tait en retard ce matin\n", "Alfred");
	printf("voici un pourcent : %%, un antislash \\, un guillemet \" et une tabulation ...\t... \n");

#endif	//SHOW_EXAMPLES

	/////////////////////////////////////////////////////////////////
	//deuxiï¿½me partie : complï¿½tion de lignes utilisant printf.
	char string[100];
	const char * user = "Alan";
	typedef struct{
		uint8_t day;
		uint8_t month;
		uint16_t year;
	}date_t;
	__unused static date_t birthday = {23, 6, 1912};

	sprintf(string, "Hello %s", user);				//1- Complï¿½tez cette ligne en utilisant la variable user
	check(string, "Hello Alan");					//interdiction de modifier cette ligne

	sprintf(string, "you were born on %d/%02d/%d", birthday.day, birthday.month, birthday.year);	//2- Complï¿½tez cette ligne en utilisant la variable birthday
	check(string, "you were born on 23/06/1912");	//interdiction de modifier cette ligne

	uint16_t drink = 0xCAFE;
	sprintf(string, "%x" ,drink);					//3- Comply¿½tez cette ligne en utilisant la variable drink
	check(string, "cafe");							//interdiction de modifier cette ligne
}



//------------------------------ETAPE 3------------------------------------
void operators_training(__unused uint32_t * data, uint32_t *answers)
{
	//cet exercice a pour but de vous faire pratiquer :
		//les opï¿½rateurs logiques
	    //les opï¿½rateurs de dï¿½calage
	    //les opï¿½rateurs logiques bit-ï¿½-bit
	    //les opï¿½rateurs arithmï¿½tiques

	//un tableau de donnï¿½es est fourni. sa taille est de 4 ï¿½lï¿½ments.
	//vous devez remplir le tableau answers de 9 ï¿½lï¿½ments correspondant aux rï¿½sultats des opï¿½rations demandï¿½es.
	//observez l'ï¿½tat des valeurs correspondantes avec le dï¿½bogueur, en mode pas ï¿½ pas

						//remplir cette rï¿½ponse avec :
	answers[0] = data[0]<<1;		//data[0] dï¿½calï¿½e de 1 bits vers la gauche

	answers[1] = data[0]>>4;		//data[0] dï¿½calï¿½e de 4 bits vers la droite

	answers[2] = data[0]|data[1];		//data[0] ou logique bit ï¿½ bit data[1]

	answers[3] = data[0]||data[1];		//ou logique entre data[0] et data[1] (attention, ce n'est pas un ou logique bit ï¿½ bit !)

	answers[4] = data[0]&data[1];		//data[0] et logique bit ï¿½ bit data[1]

	answers[5] = data[0]^data[1];		//data[0] ou exclusif logique bit ï¿½ bit data[1]

	answers[6] = data[2]/data[3];		//data[2] divisï¿½ par data[3]

	answers[7] = data[2]%data[3];		//data[2] modulo data[3]

	answers[8] = data[2]*data[3];		//data[2] multipliï¿½ par data[3]

	__unused uint8_t higher = (data[4]>>24) &0xFF;	//8 bits de poids fort de data[4] en utilisant un masque & et un dï¿½calage
	__unused uint8_t high = (data[4]>>16) &0xFF;	//8 bits de poids suivants	de data[4] en utilisant un masque & et un dï¿½calage
	__unused uint8_t low = 	(data[4]>>8)	&0xFF;	//8 bits de poids suivants 	de data[4] en utilisant un masque & et un dï¿½calage
	__unused uint8_t lower = data[4] &0xFF;	//8 bits de poids faible 	de data[4] en utilisant un masque & et un dï¿½calage
	answers[9] = (lower << 24) | (low << 16) | (high << 8) | (higher);		//4 octets lower--low--high--higher DANS CET ORDRE en utilisant un masque | et des dï¿½calages
}



//------------------------------ETAPE 4------------------------------------
/*
 * Cette fonction remplace les majuscules par des minuscules dans la chaine de caractï¿½res s.
 * Vous devez utiliser une boucle pour parcourir la chaine, jusqu'ï¿½ rencontrer le caractï¿½re de fin de chaine '\0'.
 * Pour remplacer une majuscule par une minuscule, vous pouvez ajouter 'a'-'A'.
 */
void maj2min(char *s)
{
	for(int8_t b =0; s[b] != '\0'; b++){
		if(s[b] >= 'A' && s[b] <= 'Z'){
			s[b] +=  'a'-'A';
		}
	}

}


//------------------------------ETAPE 5------------------------------------
/*
 * Cette fonction rempli les size premiï¿½res cases du tableau tab, avec les lettres de l'alphabet en minuscule.
 * premiï¿½re case : 'a'
 * deuxiï¿½me case : 'b'
 * ...
 * Vous devez utiliser une boucle for pour remplir ce tableau.
 */
void full_tab_with_letters(char *tab, uint8_t size)
{

	for(int8_t b = 0; b < size; b++){
		tab[b] = 'a' + b;
	}
}


//------------------------------ETAPE 6------------------------------------
void play_with_pointer(uint32_t * data)
{
	//Voici un tableau tab de 20 ï¿½lï¿½ments de type char
	char tab[20];

	//utilisez sprintf pour remplir ce tableau avec votre prï¿½nom
	sprintf(tab, "Chrys Brian");

	//crï¿½ez un pointeur p de type char *
	char *p;

	//faites en sorte que ce pointeur pointe vers la 3ï¿½me case du tableau (! c'est donc la case nï¿½2 !)
	p = &tab[2];

	//dï¿½commentez ce printf qui affiche l'adresse du tableau et l'adresse de la case pointï¿½e par p
	printf("@tab=%lx   | @p=%lx\n", (uint32_t)tab, (uint32_t)p);

	//ï¿½crivez un printf qui affiche le contenu de la case pointï¿½e par p
	printf("%c", *p);

	//ï¿½crivez un printf qui affiche le contenu de la chaine de caractï¿½res "partielle" pointï¿½e par p
	printf("%s\n", p);


	//ï¿½crivez un printf qui affiche l'adresse de la variable p et constatez qu'on en a rien ï¿½ foutre de l'adresse d'un pointeur

	printf("%x", &p);
    //voici un type de pointeur sur fonction renvoyant du void et admettant un char * en paramï¿½tre !
	typedef void (*fun_ptr_t)(char *);

	//crï¿½ez une variable pf de ce type pointeur sur fonction.
	fun_ptr_t pf;

	//faites pointer pf sur la fonction maj2min
	pf = &maj2min;

	//dï¿½commentez cet appel ï¿½ la fonction pointï¿½e par pf :
	char * str = (char*)data[5];
	pf(str);

	//dï¿½commentez les lignes suivantes sans les modifier.
	data[0] = U32FROMU8(tab[0], tab[1], tab[2], tab[3]);
	data[1] = (uint32_t)*p;
	data[2] = (uint32_t)p;
	data[3] = (uint32_t)tab;
	data[4] = sizeof(tab);
	data[6] = (uint32_t)pf;

}


//------------------------------ETAPE 7------------------------------------

#define ENABLE_A_PORTION_OF_CODE    1
#define TARTAMPION
#define TARTAMPION
#define MAX_NUMBER_OF_SARDIN_IN_THE_BOX 5
#define GUESS_THIS_NUMBER 3

//vous ne devez RIEN modifier DANS cette fonction.
//Faites en sortes, en dï¿½finissant et en modifiant des macros prï¿½processeur, ci-dessus, de produire le rï¿½sultat attendu.
// rï¿½sultat attendu :
		// data[0] = 0
        // data[1] = 5
		// data[2] = 0xCACADEFE
        // data[3] = 8
void play_with_macros(uint32_t * data)
{
#if !ENABLE_A_PORTION_OF_CODE												//ne pas modifier cette ligne !
	data[0] = 0;															//ne pas modifier cette ligne !
	data[1] = 0;															//ne pas modifier cette ligne !
	data[2] = 0;															//ne pas modifier cette ligne !
	data[3] = 0;															//ne pas modifier cette ligne !
	data[4] = 0;															//ne pas modifier cette ligne !
#else																		//ne pas modifier cette ligne !
	data[1] = MAX_NUMBER_OF_SARDIN_IN_THE_BOX;								//ne pas modifier cette ligne !
	#ifndef	TARTAMPION														//ne pas modifier cette ligne !
		data[2] = 0xDEFECA;													//ne pas modifier cette ligne !
	#else																	//ne pas modifier cette ligne !
		data[2] = 0xCACADEFE;												//ne pas modifier cette ligne !
	#endif																	//ne pas modifier cette ligne !
	data[3] = sizeof(uint32_t) + sizeof(char) + GUESS_THIS_NUMBER;			//ne pas modifier cette ligne !
#endif
}



