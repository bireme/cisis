#ifndef CICGI_HPP
#define CICGI_HPP

#include <cisisx.hpp>

#define CICG1		0

class _YOURCLASS CGIClass
{
 public:

  CISISX * cisisxp;

  CGIClass(CISISX *parm_cisisxp) { cisisxp = parm_cisisxp; }

  int cicgi0(int *argc,
             char *argv[],
             char **wwwp,
             UWORD taghxsub,
             char *taghpfxp);

#if CICG1
  int cicgi1(int *argc,
             char *argv[],
             CICGI_ENTRY entries[],
             int nentries,
             char *cicgi_p)
#endif /* CICG1 */

  char *gettmpnam(char *namp);
  int cigt_function(char *varp, char *action);

};

#endif /* CICGI_HPP */
