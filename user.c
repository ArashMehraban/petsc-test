#include "user.h"

#undef __FUNCT__
#define __FUNCT__ "processUserOptions"
PetscErrorCode processUserOptions(MPI_Comm comm, AppCtx *userOptions)
{
  PetscErrorCode  ierr;
  PetscBool       flg;

  PetscFunctionBeginUser;

  userOptions->filename[0] = '\0';  // mesh file to read
 
  //start setting user options from command line
  ierr = PetscOptionsBegin(comm, "", "Test Dirichlet Boundary ", "DMPLEX");CHKERRQ(ierr);

  //set the option (-f) to get a filename from user: mpiexec -n 1 ./main  -f filename
  ierr = PetscOptionsString("-f", "Exodus.II filename to read", "main.c", userOptions->filename, userOptions->filename, sizeof(userOptions->filename), &flg);CHKERRQ(ierr);
  #if !defined(PETSC_HAVE_EXODUSII)
    if(flg)  SETERRQ(comm, PETSC_ERR_ARG_WRONG, "This option requires ExodusII support. Reconfigure your Arch with --download-exodusii");
  #endif

  PetscOptionsEnd();
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "dmDistDMfromFile"
PetscErrorCode dmDistDMfromFile(MPI_Comm comm, AppCtx user, DM *dm){

  PetscErrorCode  ierr;
  const char      *filename = user.filename;
  PetscBool       interpolate = PETSC_TRUE;
  DM              distributedMesh = NULL;


  PetscFunctionBeginUser;

  //Create a dmplex from Exodus-II file
  ierr = DMPlexCreateFromFile(comm, filename, interpolate, dm);CHKERRQ(ierr);

  /* Distribute mesh over processors */
  ierr = DMPlexDistribute(*dm, 0, NULL, &distributedMesh);CHKERRQ(ierr);
	if (distributedMesh) {
		ierr = DMDestroy(dm);CHKERRQ(ierr);
		*dm  = distributedMesh;
	}

  PetscFunctionReturn(0);
}
#undef __FUNCT__
#define __FUNCT__ "dmCreateSection"
PetscErrorCode dmCreateSection(MPI_Comm comm, DM *dm, PetscInt numFields, PetscInt dof){

  PetscErrorCode ierr;
  PetscSection   section;
  PetscInt       pStart, pEnd;

  PetscFunctionBeginUser;

    //Initialize petsc section
    ierr = PetscSectionCreate(comm,&section);CHKERRQ(ierr);
    //Set the number of fields from userCtx
  	ierr = PetscSectionSetNumFields(section, numFields);CHKERRQ(ierr);
    //get the chart from dmplex
  	ierr = DMPlexGetChart(*dm, &pStart, &pEnd);CHKERRQ(ierr);
    //set the chart for each processor from pStart to pEnd
  	ierr = PetscSectionSetChart(section,pStart, pEnd);CHKERRQ(ierr);
    //set dof for all nodes in pStart to pEnd
    for(PetscInt i=pStart; i < pEnd; i++){
        ierr = PetscSectionSetDof(section, i, dof);CHKERRQ(ierr);
    }
    //Calculate offsets based upon the number of degrees of freedom for each point.
  	ierr = PetscSectionSetUp(section);CHKERRQ(ierr);
    //Set the section for dm
    ierr = DMSetDefaultSection(*dm,section);CHKERRQ(ierr);
    PetscFunctionReturn(0);

}


