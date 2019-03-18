#include "mem.h"
#include "common.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

// constante définie dans gcc seulement
#ifdef __BIGGEST_ALIGNMENT__
#define ALIGNMENT __BIGGEST_ALIGNMENT__
#else
#define ALIGNMENT 16
#endif

//macro pour l'alignement
#define ALIGN(val, align)  (((val) + ((align) - 1)) &~ ((align) - 1))

struct fb {
	size_t size;
	struct fb* next;
	/* ... */
};

//Déclaration de notre liste
struct fb *first_fb;


void mem_init(void* mem, size_t taille)
{
	assert(mem == get_memory_adr());
	assert(taille == get_memory_size());
	/* ... */

	//first_fb = {get_memory_size() - sizeof(struct fb *) + (size_t)1, liste};

	//Initialisation de la liste avec la taille donnée en paramètres et un pointeur vers null
	first_fb = (struct fb *) mem;
	first_fb->size = taille;
	first_fb->next = NULL;

	//Création d'une tête de liste qui pointe vers la liste, de taille 1
	//first_fb.size = get_memory_size() - sizeof(struct fb *) + 1;
	//first_fb.next = liste;

	mem_fit(&mem_fit_first);
}

void mem_show(void (*print)(void *, size_t, int)) {
	/* ... */
	//curseur de la zone courante
	void* courant = get_memory_adr();
	//Valeur du pointeur de fin de mémoire
	void *fin_Mem = courant + get_memory_size();
	//liste de blocs libres, pointe vers le prochain bloc libre
	struct fb *libreSuivant = first_fb;
	//Le booléen free indique si le bloc est occupé(0) ou pas
	int free;
	while (courant < fin_Mem)
	{
			//Comme on parcourt les zones libres, on initalise free à 0
		free = 0;
		//On récupère la taille du bloc afin de pouvoir l'afficher en fin de fonction
		size_t dimension_zone = ((struct fb*)courant)->size;
		if(courant == libreSuivant)
		{
			free = 1;
			libreSuivant = libreSuivant->next;
		}
		print(courant, dimension_zone, free);
		courant = courant + dimension_zone;
	}
}

static mem_fit_function_t *mem_fit_fn;
void mem_fit(mem_fit_function_t *f) {
	mem_fit_fn=f;
}


void *mem_alloc(size_t taille) {
	/* ... */
	__attribute__((unused)) /* juste pour que gcc compile ce squelette avec -Werror */
//	struct fb *fb=mem_fit_fn(/*...*/NULL, /*...*/0);
	/* ... */

	//Initialisation de la taille recherchée dans le bloc mémoire
	//Ajout des métadonnées en fonction de l'alignement requis
	size_t dimension_Zo;
	dimension_Zo = ALIGN(sizeof(size_t), ALIGNMENT) + ALIGN(taille, ALIGNMENT);

	struct fb *liste = first_fb;
	//Variable pour récupérer le résultat de mem_fit_fn
	//Indique si l'allocation a réussi ou échoué (renvoie NULL dans ce cas précis)
	struct fb* newZo;

	newZo = mem_fit_fn(liste, dimension_Zo);

	if(newZo != NULL)
	{
		return newZo;
	}

	size_t espaceLibre;
	struct fb *nouvelleZL, *pointeur;
	//struct fb *zoneOccupee;
	espaceLibre = newZo->size - dimension_Zo;

	if(espaceLibre >= sizeof(struct fb*))
	{
		//initialisation d'une nouvelle zone libre de la liste ainsi que de la zone qui va être occupée
		nouvelleZL = (struct fb *)((void *) taille);
		nouvelleZL->size = espaceLibre;
		nouvelleZL->next = newZo->next;
		pointeur = newZo;
		//zoneOccupee->size = taille;
		newZo->next = newZo->next + (size_t)taille;

		return pointeur;
	}

	else
	{
		pointeur = newZo;
		//zoneOccupee->size = espaceLibre;
		newZo->next = (newZo->next)->next;
		return pointeur;
	}

	return NULL;
}


void mem_free(void* mem) {
		/*
    //Pour garder en mémoire la zone libre précédente
    struct fb *derniere_ZL;
    struct fb *liste = first_fb;
    //Pour garder en mémoire le pointeur vers cette zone libre
    void *pointeurZL_prec = get_memory_adr();
    while((liste->next != NULL) || (liste->next < mem))
    {
        derniere_ZL = liste;
        pointeurZL_prec = pointeurZL_prec + derniere_ZL->size;
        liste = liste->next;
		}
		if(liste->next != NULL)
		{
			//Cas où la zone Libre précédente se situe juste à côté et qu'il faut les fusionner
			if(liste == mem && pointeurZL_prec == liste)
			{
			    struct fb *newZL;
			    newZL->size = liste->size + derniere_ZL->size;
			    newZL->next = liste->next;
			    liste->next = newZL;
					liste = liste->next;
			}
			//Cas où il n'y a pas de zone libre avant à fusionner
			//On regarde si la zone suivante est libre
			if (liste->next == mem + mem->size)
			{
			    struct fb *newZL;
			    newZL->size = mem->size + liste->size;
			    newZL->next = (liste->next)->next;
			    liste->next = newZL;
			}
			*/
}


struct fb* mem_fit_first(struct fb *list, size_t size) {
	while(list->next != NULL)
	{
		//On cherche s'il y a un bloc libre qui puisse contenir la taille demandée par l'utilisateur
		if(((list->next)->size) >= size)
		{
			return (list->next);
		}
		list = list->next;
	}
	return NULL;
}

/* Fonction à faire dans un second temps
 * - utilisée par realloc() dans malloc_stub.c
 * - nécessaire pour remplacer l'allocateur de la libc
 * - donc nécessaire pour 'make test_ls'
 * Lire malloc_stub.c pour comprendre son utilisation
 * (ou en discuter avec l'enseignant)
 */
size_t mem_get_size(void *zone) {
	/* zone est une adresse qui a été retournée par mem_alloc() */

	/* la valeur retournée doit être la taille maximale que
	 * l'utilisateur peut utiliser dans cette zone */
	return 0;
}

/* Fonctions facultatives
 * autres stratégies d'allocation
 */
struct fb* mem_fit_best(struct fb *list, size_t size) {
	return NULL;
}

struct fb* mem_fit_worst(struct fb *list, size_t size) {
	return NULL;
}
