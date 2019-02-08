Note1: I have put my PETSC_DIR and PETSC_ARCH in the makefile. You need to modify the path to yours.
Note2: I use PETSc 3.7.5 but this code should work with any PETSc version above that.

Once you modify the makefile and figure out your PETSC_DIR and PETSC_Arch, you can use this makefile to compile the code:
make all

Run like this: 
mpiexec -n 1 ./main -f cube.exo

The way I make this code:
1. I have made a very simple struct called user. This user struct has only one field that accepts a filename from command line.
 
2. I have three functions associated with user object:
    I) processUserOptions function that let the user specifies a filename from command line
   II) dmDisDMfromFile function that creates a dmplex object from the Exodus-II file (mesh file) that was read into PETSc and distributes the mesh over processors
  III) dmCreateSection that creates a PetscSection for the DM object
  (The above functions are used lines 22-24 in main.c)

3. Then I used the DMGetLabelIdIS function to get the labels I had created in the Cubit/Trelis file for the nodesets (line 25 in main.c) My labes were 998 and 999 
   for the two nodes sets in cube.exo file

The rest of code is self explanetory.

Here is the problem:
PETSc can only read low-order mesh (For example Q1 elements). So if you need a Q2 mesh, you read the Q1 mesh into Petsc but you can use the interpolate=PETSC_TRUE when 
you use DMPlexCreateFromFile(comm, filename, interpolate,dm). Please look at line 33 and line 40 in user.c file. This will create dofs on edges, faces and center of each
element. The problem is that the interpolated mesh (Q2 mesh) still has nodesets from the Q1 element. So If you want to put Dirichlet Boundary Values on all nodes of a face
you cannot use a nodeset. look at this 2D example:

o---------------o
|		|		
|		|
|		|     <--- This mesh is read in for example
|		|
|		|
o---------------o  

After interpolate= PETSC_TRUE is used with DMPlexCreateFromFile(comm, filename, interpolate,dm), then you have a mesh (DMplex object) like this:

o-------o-------o
|		|
|		|
o	o	o     <--- This interpolated mesh
|		|
|	      	|
o-------o-------o

So, your Dirichlet boundary values are going to be on nodes of the Q1 mesh above but you need it on all the dofs on Q2 mesh. 

So we should use sidesets in Cubit/Trelis instead of nodeset. This way we can access all the nodes in that face and put 
 Dirichlet Boundary conditions on all the Q2 nodes.

I have provided a very small mesh file that has two 3D elements, called cube2.exo that has 2 sidesets in it. We can use that to test the code.
