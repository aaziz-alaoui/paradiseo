// -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-

// "edge_xover.cpp"

// (c) OPAC Team, LIFL, 2003-2006

/* LICENCE TEXT
   
   Contact: paradiseo-help@lists.gforge.inria.fr
*/

#include <assert.h>

#include <utils/eoRNG.h>

#include "edge_xover.h"
#include "route_valid.h"

#define MAXINT 1000000

void
EdgeXover :: build_map (const Route & __par1, const Route & __par2) 
{
  
  unsigned int len = __par1.size () ;
  
  /* Initialization */
  _map.clear () ;
  _map.resize (len) ;
  
  for (unsigned int i = 0 ; i < len ; i ++) 
    {
      _map [__par1 [i]].insert (__par1 [(i + 1) % len]) ;
      _map [__par2 [i]].insert (__par2 [(i + 1) % len]) ;
      _map [__par1 [i]].insert (__par1 [(i - 1 + len) % len]) ;
      _map [__par2 [i]].insert (__par2 [(i - 1 + len) % len]) ;
    }
  
  visited.clear () ;
  visited.resize (len, false) ;
}

void
EdgeXover :: remove_entry (unsigned int __vertex, std :: vector <std :: set <unsigned int> > & __map) 
{
  
  std :: set <unsigned int> & neigh = __map [__vertex] ;

  for (std :: set <unsigned int> :: iterator it = neigh.begin () ; it != neigh.end () ; it ++)
    {
      __map [* it].erase (__vertex) ; 
    }
      
}

void
EdgeXover :: add_vertex (unsigned int __vertex, Route & __child) 
{
  visited [__vertex] = true ;
  __child.push_back (__vertex) ;    
  remove_entry (__vertex, _map) ; /* Removing entries */    
}

void
EdgeXover :: cross (const Route & __par1, const Route & __par2, Route & __child) {
  
  build_map (__par1, __par2) ;
  
  unsigned int len = __par1.size () ;
 
  /* Go ! */
  __child.clear () ;
  
  unsigned int cur_vertex = rng.random (len) ;
  
  add_vertex (cur_vertex, __child) ;

  for (unsigned int i = 1 ; i < len ; i ++) {
    
    unsigned int len_min_entry = MAXINT ;
    
    std :: set <unsigned int> & neigh = _map [cur_vertex] ;
    
    for (std :: set <unsigned int> :: iterator it = neigh.begin () ; it != neigh.end () ; it ++) 
      {      
	unsigned int l = _map [* it].size () ;
	if (len_min_entry > l)
	  {
	    len_min_entry = l ;
	  }
      }
    
    std :: vector <unsigned int> cand ; /* Candidates */
    
    for (std :: set <unsigned> :: iterator it = neigh.begin () ; it != neigh.end () ;  it ++) 
      {      
	unsigned int l = _map [* it].size () ;
	if (len_min_entry == l)
	  {
	    cand.push_back (* it) ;
	  }
      }
       
    if (! cand.size ()) 
      {
	
	/* Oh no ! Implicit mutation */      
	for (unsigned int j = 0 ; j < len ; j ++)
	  {
	    if (! visited [j])
	      {
		cand.push_back (j) ;
	      }
	  }
      }
    
    cur_vertex = cand [rng.random (cand.size ())] ;
    
    add_vertex (cur_vertex, __child) ;
  } 
}

bool
EdgeXover :: operator () (Route & __route1, Route & __route2) 
{
  
  // Init. copy
  Route par [2] ;
  par [0] = __route1 ;
  par [1] = __route2 ;
  
  cross (par [0], par [1], __route1) ;
  cross (par [1], par [0], __route2) ;
  
  assert (valid (__route1)) ;
  assert (valid (__route2)) ;

  __route1.invalidate () ;
  __route2.invalidate () ;

  return true ;
}