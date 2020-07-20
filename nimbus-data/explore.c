/*
 *  This example illustrates how to navigate an HDF5 file with simple structure
 *  limited to groups of groups of datasets, and to extract the data sets for 
 *  export to another application and for use in heterogeneous compute environments.
 *
 *  The motivation is to transport TensorFlow weights from Python to Vitis/FPGA/MPSoC
 * 
 *  Based on example code from the HDF5 distribution version 1.12.0
 *
 * h5ex_g_traverse.c : method to walk the data structure using H5Literate
 *
 *
 * Learning points about building/using HDF5 from the C API.
 * 
 * 1. Basic attempt to build h5ex_g_traverse.s using cmake in directory results in
 * compilation error for too many arguments provided to function H5Ovisit1.  This could
 * be version related.  The example references HDF5 Library version 1.8.
 * 2. Resolution https://stackoverflow.com/questions/18557237/hdf5-example-not-working and the solution is to provide -DH5_USE18_API ?  This gives different errors locally.
 * 3. Unknown type name H5L_info1_t : should be H5L_info_t ?
 * 4. In general, there are some oddities with naming that suggests an API naming convention for backwards compatibility that I don't fully understand (lots of suffix numerals).
 * 5. Need to generate some index/auto-help - could be time to learn about the tools independent indexing system for API management, if I am to do much real dev work.
 * 6. From H5version.h we find lots of #define which remap the API calls to specific version of the API calls with some very fine granular system.   This is rather a mess, in that it suggests legacy code will have specific number calls, which is not very portable.
 * 7. See https://support.hdfgroup.org/HDF5/doc/TechNotes/Version.html
 * 8. Actually - just compiles if using the out of the box compile/install 1.12.0 version
 */

#include <stdio.h>

/* #define H5_USE18_API */

#include "hdf5.h"

/*
 * Define operator data structure type for H5Literate callback.
 * During recursive iteration, these structures will form a
 * linked list that can be searched for duplicate groups,
 * preventing infinite recursion.
 */
struct opdata {
  unsigned        recurs;         /* Recursion level.  0=root */
  struct opdata   *prev;          /* Pointer to previous opdata */
  haddr_t         addr;           /* Group address */
};

/*
 * Operator function to be called by H5Literate.
 */
herr_t op_func (hid_t loc_id, const char *name, const H5L_info1_t *info,
		void *operator_data);

/*
 * Function to check for duplicate groups in a path.
 */
int group_check (struct opdata *od, haddr_t target_addr);


void usage(char * h5file, char *error) {
  printf("Usage : explore <hdf5filename>\n\n\tLight version of h5dump.  Basis for extracting bias and kernel Tensorflow data to other programs\n");
  if (h5file == NULL) {
    printf("No hdf5filename provided\n");
    if(error != NULL) { printf("%s\n", error);}
    return;
  }
}

int main(int argc, char* argv[]) {

  if (argc < 2) {
    usage(NULL, "must provide an hdf5 file as argument");
  }

  hid_t           file;           /* Handle */
  herr_t          status;
  H5O_info1_t      infobuf;
  struct opdata   od;
  
  /*
   * Open file and initialize the operator data structure.
   */
  file = H5Fopen (argv[1], H5F_ACC_RDONLY, H5P_DEFAULT);
  status = H5Oget_info2 (file, &infobuf, H5O_INFO_ALL);
  od.recurs = 0;
  od.prev = NULL;
  od.addr = infobuf.addr;
  
  /*
   * Print the root group and formatting, begin iteration.
   */
  printf ("/ {\n");
  status = H5Literate1 (file, H5_INDEX_NAME, H5_ITER_NATIVE, NULL, op_func,
			(void *) &od);
  printf ("}\n");
  
  /*
   * Close and release resources.
   */
  status = H5Fclose (file);
  
  return 0;
}

/************************************************************

  Operator function.  This function prints the name and type
  of the object passed to it.  If the object is a group, it
  is first checked against other groups in its path using
  the group_check function, then if it is not a duplicate,
  H5Literate is called for that group.  This guarantees that
  the program will not enter infinite recursion due to a
  circular path in the file.

************************************************************/
herr_t op_func (hid_t loc_id, const char *name, const H5L_info1_t *info,
		void *operator_data)
{
  herr_t          status, return_val = 0;
  H5O_info1_t     infobuf;
  struct opdata   *od = (struct opdata *) operator_data;
  /* Type conversion */
  unsigned        spaces = 2*(od->recurs+1);
  /* Number of whitespaces to prepend
     to output */

  /*
   * Get type of the object and display its name and type.
   * The name of the object is passed to this function by
   * the Library.
   */
  status = H5Oget_info_by_name2 (loc_id, name, &infobuf, H5O_INFO_ALL, H5P_DEFAULT);
  printf ("%*s", spaces, "");     /* Format output */
  switch (infobuf.type) {
  case H5O_TYPE_GROUP:
    printf ("Group: %s {\n", name);
      
    /*
     * Check group address against linked list of operator
     * data structures.  We will always run the check, as the
     * reference count cannot be relied upon if there are
     * symbolic links, and H5Oget_info_by_name always follows
     * symbolic links.  Alternatively we could use H5Lget_info
     * and never recurse on groups discovered by symbolic
     * links, however it could still fail if an object's
     * reference count was manually manipulated with
     * H5Odecr_refcount.
     */
    if ( group_check (od, infobuf.addr) ) {
      printf ("%*s  Warning: Loop detected!\n", spaces, "");
    }
    else {
	
      /*
       * Initialize new operator data structure and
       * begin recursive iteration on the discovered
       * group.  The new opdata structure is given a
       * pointer to the current one.
       */
      struct opdata nextod;
      nextod.recurs = od->recurs + 1;
      nextod.prev = od;
      nextod.addr = infobuf.addr;
      return_val = H5Literate_by_name1 (loc_id, name, H5_INDEX_NAME,
					H5_ITER_NATIVE, NULL, op_func, (void *) &nextod,
					H5P_DEFAULT);
    }
    printf ("%*s}\n", spaces, "");
    break;
  case H5O_TYPE_DATASET:
    printf ("Dataset: %s\n", name);
    break;
  case H5O_TYPE_NAMED_DATATYPE:
    printf ("Datatype: %s\n", name);
    break;
  default:
    printf ( "Unknown: %s\n", name);
  }

  return return_val;
}


/************************************************************

  This function recursively searches the linked list of
  opdata structures for one whose address matches
  target_addr.  Returns 1 if a match is found, and 0
  otherwise.

************************************************************/
int group_check (struct opdata *od, haddr_t target_addr)
{
  if (od->addr == target_addr)
    return 1;       /* Addresses match */
  else if (!od->recurs)
    return 0;       /* Root group reached with no matches */
  else
    return group_check (od->prev, target_addr);
  /* Recursively examine the next node */
}
