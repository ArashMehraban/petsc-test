#ifndef __USER__H
#define __USER__H

#include<petscdmplex.h>

typedef struct{
  char  filename[PETSC_MAX_PATH_LEN]; //optional exodusII file
}AppCtx;

//Process the user options. For this test user only specifies a filename (Exodus-II) 
PetscErrorCode processUserOptions(MPI_Comm comm, AppCtx *userOptions);
//Create a dmplex from Exodus-II file and distribute the mesh over processors
PetscErrorCode dmDistDMfromFile(MPI_Comm comm, AppCtx user, DM *dm);
//Create Petsc Section for the DM object
PetscErrorCode dmCreateSection(MPI_Comm comm, DM *dm, PetscInt numFields, PetscInt dof);


#endif //end of __USER__H
