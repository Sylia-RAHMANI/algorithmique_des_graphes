#include <stdlib.h>
#include <stdio.h>
#include "graphe-4.h"
#include "parcours.h"
#include "conteneur_sommets.h"
#include "pile.h"
#include "file.h"
struct parcours *pc_init(graphe *g, conteneur_sommets *cs, int *prio)
{
	if (!g)
	{
		printf("graphe vide rien a parcourir\n");
		return NULL;
	}
	struct parcours *p = malloc(sizeof(struct parcours));
	if (p == NULL)
		return NULL;
	p->mon_graphe = g;
	p->conteneur = cs;
	p->ordr_preffixe = file_creer(g->n);
	p->ordr_suffixe = file_creer(g->n);
	p->tab = calloc(g->n, sizeof(int));
	p->distance_depuis_r = calloc(g->n, sizeof(int));
	p->explore = calloc(g->n, sizeof(int));
	p->t = calloc(g->n, sizeof(int));
	for (int i = 0; i < g->n; i++)
		p->t[i] = i;
	if (prio == NULL)
	{
		p->prio = calloc(g->n, sizeof(int));
		for (int i = 0; i < (p->mon_graphe)->n; i++)
			p->prio[i] = i;
	}
	else
		p->prio = prio;
	p->arbo = graphe_creer(g->n, 1);
	p->existe_circuit = 0;
	return p;
}

void pc_detruire(struct parcours *p)
{
	if (p)
	{
		file_detruire(p->ordr_preffixe);
		file_detruire(p->ordr_suffixe);
		free(p->explore);
		free(p->tab);
		free(p->t);
		free(p->distance_depuis_r);
		free(p);
	}
}

int pc_choisir_racine(struct parcours *p)
{
	if (p)
	{
		int r = 0;
		for (int i = 0; i < p->mon_graphe->n; i++)
		{
			r = p->prio[i];
			if (p->explore[r] != 1)
				return r;
		}
		return r;
	}
	return -1;
}

int pc_est_fini(struct parcours *p)
{
	return file_taille(p->ordr_suffixe) == p->mon_graphe->n;
}

void pc_marquer_comme_visite(struct parcours *p, int sommet)
{
	p->tab[sommet] = 1;
}

int pc_est_visite(struct parcours *p, int sommet)
{
	return p->tab[sommet] == 1;
}

void pc_marquer_comme_visite_depuis(struct parcours *p, int sommet, int depuis)
{
	p->t[sommet] = depuis;
}

void pc_marquer_comme_explore(struct parcours *p, int sommet)
{
	p->explore[sommet] = 1;
}

int pc_conteneur_est_vide(struct parcours *p)
{
	return (cs_est_vide(p->conteneur));
}

void pc_ajouter_dans_conteneur(struct parcours *p, int sommet)
{
	cs_ajouter(p->conteneur, sommet);
}

void pc_supprimer_du_conteneur(struct parcours *p)
{
	cs_supprimer(p->conteneur);
}

int pc_choisir_dans_conteneur(struct parcours *p)
{
	return (cs_choisir(p->conteneur));
}

msuc *pc_prochain_msuc(struct parcours *p, int sommet)
{
	return graphe_get_prem_msuc(p->mon_graphe, sommet);
}

msuc *maillon_suivant(struct parcours *p, int sommet)
{
	msuc *m = pc_prochain_msuc(p, sommet);
	while (m)
	{
		if (pc_est_visite(p, m->sommet))
			m = msuc_suivant(m);
		else
			return m;
	}
	return m;
}

void pc_parcourir_depuis_sommet(struct parcours *p, int r)
{
	if (p)
	{
		pc_ajouter_dans_conteneur(p, r);
		pc_marquer_comme_visite(p, r);
		msuc *m = NULL;
		while (!cs_est_vide(p->conteneur))
		{
			int v = pc_choisir_dans_conteneur(p);
			m = pc_prochain_msuc(p, v);
			msuc *k = NULL;
			while (m)
			{
				if ((p->explore[m->sommet] != 1) && pc_est_visite(p, m->sommet))
				{
					p->existe_circuit = 1;
				}
				if (!pc_est_visite(p, m->sommet))
				{
					k = m;
					m = NULL;
				}
				m = msuc_suivant(m);
			}
			if (k != NULL && !pc_est_visite(p, k->sommet))
			{
				pc_marquer_comme_visite_depuis(p, k->sommet, v);
				p->distance_depuis_r[k->sommet] = p->distance_depuis_r[v] + 1;
				pc_marquer_comme_visite(p, k->sommet);
				file_enfiler(p->ordr_preffixe, k->sommet);
				pc_ajouter_dans_conteneur(p, k->sommet);
				graphe_ajouter_arc(p->arbo, v, k->sommet, k->valeur);
			}
			else
			{
				pc_marquer_comme_explore(p, v);
				file_enfiler(p->ordr_suffixe, v);
				pc_supprimer_du_conteneur(p);
			}
		}
	}
}

void pc_parcourir(struct parcours *p)
{
	while (!pc_est_fini(p))
		pc_parcourir_depuis_sommet(p, pc_choisir_racine(p));
}
