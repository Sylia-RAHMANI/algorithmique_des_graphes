#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graphe_parcours.h"
#include "graphe-4.h"
#include "parcours.h"
#include "conteneur_sommets.h"

int graphe_parcours_profondeur(graphe *g, graphe **arbo, int **suff, int *prio)
{
  if (!g)
    return -1;
  conteneur_sommets *cs = cs_creer_pile(g->n);
  if (!cs)
    return -1;
  struct parcours *p = pc_init(g, cs, prio);
  if (!p)
    return -1;
  pc_parcourir(p);
  *suff = malloc(g->n * sizeof(int));
  if (!(*suff))
    return -1;
  for (int i = 0; i < g->n; i++)
    *(*suff + i) = file_defiler(p->ordr_suffixe);
  *arbo = p->arbo;
  cs_detruire(cs);
  free(p->prio);
  pc_detruire(p);

  return 0;
}

int graphe_parcours_largeur(graphe *g, graphe **arbo, int **suff, int *prio)
{

  if (!g)
    return -1;
  conteneur_sommets *cs = cs_creer_file(g->n);
  if (!cs)
    return -1;
  struct parcours *p = pc_init(g, cs, prio);
  if (!p)
    return -1;
  pc_parcourir(p);
  *suff = malloc(g->n * sizeof(int));
  if (!(*suff))
    return -1;
  for (int i = 0; i < g->n; i++)
    *(*suff + i) = file_defiler(p->ordr_suffixe);
  *arbo = p->arbo;
  cs_detruire(cs);
  free(p->prio);
  pc_detruire(p);
  return 0;
}

int graphe_parcours_larg_ou_prof(graphe *g, graphe **arbo, int **suff, int *prio)
{

  if (!g)
    return -1;
  conteneur_sommets *cs = cs_creer_pile_ou_file(g->n);
  if (!cs)
    return -1;
  struct parcours *p = pc_init(g, cs, prio);
  if (!p)
    return -1;
  pc_parcourir(p);
  *suff = malloc(g->n * sizeof(int));
  if (!(*suff))
    return -1;
  for (int i = 0; i < g->n; i++)
    *(*suff + i) = file_defiler(p->ordr_suffixe);
  *arbo = p->arbo;
  cs_detruire(cs);
  free(p->prio);
  pc_detruire(p);
  return 0;
}

int graphe_ordre_top(graphe *g, int **ordre_top)
{

  if (!g)
    return -1;
  conteneur_sommets *cs = cs_creer_pile(g->n);
  if (!cs)
    return -1;
  struct parcours *p = pc_init(g, cs, NULL);
  if (!p)
    return -1;
  pc_parcourir(p);
  if (p->existe_circuit == 1)
  {
    *ordre_top = NULL;
    cs_detruire(p->conteneur);
    graphe_liberer(p->arbo);
    free(p->prio);
    pc_detruire(p);
    return -2;
  }
  else
  {

    *ordre_top = malloc(g->n * sizeof(int));
    if (!(*ordre_top))
      return -1;
    for (int i = g->n - 1; i >= 0; i--)
      *(*ordre_top + i) = file_defiler(p->ordr_suffixe);
  }
  cs_detruire(p->conteneur);
  graphe_liberer(p->arbo);
  free(p->prio);
  pc_detruire(p);
  return 0;
}



int *graphe_distances_depuis_sommet(graphe *g, int r)
{
  int *res = NULL;
  if (!g)
    return NULL;
  conteneur_sommets *cs = cs_creer_file(g->n);
  if (!cs)
    return NULL;
  struct parcours *p = pc_init(g, cs, NULL);
  if (!p)
    return NULL;
  res = calloc(g->n, sizeof(int));
  pc_parcourir_depuis_sommet(p, r);
  for (int i = 0; i < g->n; i++)
    res[i] = p->distance_depuis_r[i];
  for (int i = 0; i < g->n; i++)
  {
    if (res[i] == 0)
    {
      res[i] = -1;
    }
  }
  res[r] = 0;
  cs_detruire(p->conteneur);
  graphe_liberer(p->arbo);
  free(p->prio);
  pc_detruire(p);
  return res;
}

int graphe_diametre(graphe *g)
{
  int max = 0;
  int *res = graphe_distances_depuis_sommet(g, 2);
  for (int i = 0; i < g->n; i++)
  {
    if (res[i] > max)
      max = res[i];
  }
  return max;
}
