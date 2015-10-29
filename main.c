#include<stdio.h>
#include <string.h>
//ne pas oublier d'inclure Sqlite
#include "sqlite3.h"

int main(int argc, char **argv)
{
	//toutes nos variables utilisées dans le programme
	sqlite3 *db;
	char *messageError = NULL;
	int codeRetour = 0;
	sqlite3_stmt *requete;
	//Requête SQL qui sera utilisée plus tard dans le programme qui permet de récupérer toutes les informations contenues dans la table contact
	char *sqlSelection = "SELECT * FROM contact";
	int id = 0;
	unsigned char *noms = NULL;
	unsigned char *prenoms = NULL;
	//on ouvre une base de données nommée "maBaseDeDonne"
	//si le fichier existe il sera utilisé
	//sinon le fichier sera automatiquement créé
	codeRetour = sqlite3_open_v2("maBaseDeDonne", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	//si codeRetour est différent de 0 on a une erreur qu'il faut gérer
	if (codeRetour){
		//affiche une erreur sur la console qui explique pourquoi la base de données n'a pas pu être ouverte (vu que certaines consoles gèrent mal les accents ,je les ai omis)
		printf("Impossible d'ouvrir la base de données : %s\n", sqlite3_errmsg(db));
		//on ferme la base de données afin de libérer la mémoire
		sqlite3_close(db);
		//on stoppe l'exécution du programme
		return 1;
	}
	//nous allons maintenant exécuter une requête sur la base de données
	/*
	 Dans cet exemple nous allons créer une table appelée contact qui contient 3 champs :
	 -id qui est un nombre auto incrémenté (c'est à dire que pour chaque ligne un nouveau numéro est automatiquement défini)
	 -noms qui est de type texte
	 -prenoms qui est de type texte
	*/
	codeRetour = sqlite3_exec(db, "CREATE TABLE contact(id INTEGER PRIMARY KEY AUTOINCREMENT, noms TEXT, prenoms TEXT)", NULL, 0, &messageError);
	//si on a une erreur avec un message d'erreur alors on libére  la mémoire pour le message d'erreur
	if (codeRetour && messageError != NULL){
		//ici on ignore l'erreur
		sqlite3_free(messageError);
		messageError = NULL;
	}
	else{
		//on va créer de nouveaux contacts uniquement si la création de la table est bien réalisée
		printf("Insertion de nouvaux contacts !\n");
		//on insère quelques valeurs dans la base de données
		sqlite3_exec(db, "INSERT INTO contact(noms, prenoms) VALUES ('Doe', 'John')", NULL, 0, &messageError);
		if (codeRetour && messageError != NULL){
			sqlite3_free(messageError);
			messageError = NULL;
		}
		//notez qu'il serait préférable d'utiliser une fonction mais pour rester le plus simple possible je vais faire ici un copier/coller
		sqlite3_exec(db, "INSERT INTO contact(noms, prenoms) VALUES ('Bond', 'James')", NULL, 0, &messageError);
		if (codeRetour && messageError != NULL){
			sqlite3_free(messageError);
			messageError = NULL;
		}
	}
	printf("Recuperation des contacts !\n");
	//deuxième méthode pour exécuter une requête sql avec cette fois une récupération des résultats
	codeRetour = sqlite3_prepare_v2(db, sqlSelection, strlen(sqlSelection), &requete, NULL);
	if (!codeRetour){
		//la préparation s'est bien déroulée on peut maintenant récupérer les résultats
		//tant qu'il y a des lignes disponibles on récupère ligne par ligne le résultat et on affiche les colonnes
		while (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW){
			//on récupère une ligne dans la table
			codeRetour = sqlite3_step(requete);
			if (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW){
				//on récupère la premiére colonne (ici le nombre id)
				id = sqlite3_column_int(requete, 0);
				//on récupère la deuxiéme colonne (ici le texte noms)
				noms = (unsigned char *)sqlite3_column_text(requete, 1);
				//on récupère la deuxième colonne (ici le texte prénoms)
				prenoms = (unsigned char *)sqlite3_column_text(requete, 2);
				//on affiche le tout dans la console
				printf("Recuperation du contact numero %d : \n Noms : %s \n Prenoms : %s \n", id, noms, prenoms);
			}
			//attention les chaines de caractères "noms" et "prenoms" n'ont pas besoin d'être libérés: SQLite le fera pour nous au prochain appel de "sqlite3_step" ou "sqlite3_finalize"
		}
		sqlite3_finalize(requete);
	}
	//on ferme la base de données afin de libérer la mémoire
	sqlite3_close(db);
	return 0;
}
