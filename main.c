static char help[] = "fem-elasticity";

#include "user.h"


#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc, char **argv)
{
  PetscErrorCode    ierr;
  DM                dm;
  AppCtx	    user; /*user-defined work context*/
  Vec               X; //res
  const char        *name="Vertex Sets";
  IS                idIS;
  PetscInt          isSize;
  const PetscInt   *ids;
  PetscInt          ns;
  PetscReal        *x;
  ierr = PetscInitialize(&argc,&argv,(char*)0,help); CHKERRQ(ierr);
  
  ierr = processUserOptions(PETSC_COMM_WORLD, &user);CHKERRQ(ierr);
  ierr = dmDistDMfromFile(PETSC_COMM_WORLD, user, &dm);CHKERRQ(ierr);
  ierr = dmCreateSection(PETSC_COMM_WORLD, &dm, 1, 3);CHKERRQ(ierr);  // 1 field and 3 dofs per node
  ierr = DMGetLabelIdIS(dm, name, &idIS);CHKERRQ(ierr);
 ierr = ISView(idIS,PETSC_VIEWER_STDOUT_SELF);CHKERRQ(ierr);
  ierr = ISGetLocalSize(idIS,&isSize);
  ierr = PetscPrintf(PETSC_COMM_SELF,"\n\nIS Size %d\n\n",isSize);CHKERRQ(ierr);
  ierr = ISGetIndices(idIS, &ids);CHKERRQ(ierr);
  ierr = DMCreateLocalVector(dm, &X);CHKERRQ(ierr);
  ierr = VecGetArray(X, &x);CHKERRQ(ierr);
  ierr = VecZeroEntries(X);CHKERRQ(ierr);
  for (ns = 0; ns < isSize; ++ns) {
    IS nodeIS;
    const PetscInt   *nodes;
    PetscInt         numNodes, n;
    PetscReal        *xI; /*Inner point*/
    ierr = DMGetStratumIS(dm, name, ids[ns], &nodeIS);
   ierr = ISView(nodeIS, PETSC_VIEWER_STDOUT_SELF);CHKERRQ(ierr);
    ierr = ISGetLocalSize(nodeIS, &numNodes);CHKERRQ(ierr);
    ierr = ISGetIndices(nodeIS, &nodes);CHKERRQ(ierr);
    for(n=0; n<numNodes; n++){

      ierr = DMPlexPointLocalRead(dm, nodes[n], x, &xI);CHKERRQ(ierr);
      xI[0] = 1.0; xI[1]=1.0; xI[2] = 1.0; 
    }   
    ierr = ISRestoreIndices(nodeIS, &nodes);CHKERRQ(ierr);
    ierr = ISDestroy(&nodeIS);CHKERRQ(ierr);
  }
  ierr = VecRestoreArray(X, &x);CHKERRQ(ierr);
 // ierr = VecView(X, PETSC_VIEWER_STDOUT_SELF);CHKERRQ(ierr);
  ierr = PetscFinalize();CHKERRQ(ierr);

return 0;
}
