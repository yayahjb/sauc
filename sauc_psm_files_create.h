/**********************************************************************
 *  sauc_psm_files_create.h                                           *
 *  Sauc utility to create the needed psm files                       *
 *  Copyright 2016 Herbert J. Bernstein, all rights reserved          *
 *                                                                    *
 *  Created by Herbert J. Bernstein on 9/18/16.                       *
 *                                                                    *
 *   You may redistribute this program under the terms of the GPL.    *
 *   Alternatively you may redistribute the functions and             *
 *   subroutines of this program as an API under themterms of the LGPL*
 **********************************************************************/

/*************************** GPL NOTICES ******************************
 *                                                                    *
 * This program is free software; you can redistribute it and/or      *
 * modify it under the terms of the GNU General Public License as     *
 * published by the Free Software Foundation; either version 2 of     *
 * (the License, or (at your option) any later version.               *
 *                                                                    *
 * This program is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      *
 * GNU General Public License for more details.                       *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * along with this program; if not, write to the Free Software        *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA           *
 * 02111-1307  USA                                                    *
 *                                                                    *
 **********************************************************************/

/************************* LGPL NOTICES *******************************
 *                                                                    *
 * This library is free software; you can redistribute it and/or      *
 * modify it under the terms of the GNU Lesser General Public         *
 * License as published by the Free Software Foundation; either       *
 * version 2.1 of the License, or (at your option) any later version. *
 *                                                                    *
 * This library is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  *
 * Lesser General Public License for more details.                    *
 *                                                                    *
 * You should have received a copy of the GNU Lesser General Public   *
 * License along with this library; if not, write to the Free         *
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,    *
 * MA  02110-1301  USA                                                *
 *                                                                    *
 **********************************************************************/


#ifndef sauc_psm_files_create_h
#define sauc_psm_files_create_h

#ifdef __cplusplus

extern "C" {
    
#endif


#include <stdio.h>
#include <string.h>
#include "pststrmgr.h"



/*  The program supports three sources of data:
 The PDB, which is provided as two files:
 entries.idx  -- the compound and source information
 PDBcelldatabase.tsv -- the cells to search on
 They will be mapped to two mmapped files
 PDBentries.dmp
 PDBcells.dmp
 The COD, which is provided as one file:
 cod.tsv
 which will be mapped to
 CODentries.dmp
 The CSD, which is provided as one file:
 CSDcelldatabase.tsv
 which will be mapped to
 CSDcelldatabase.dmp
 
 In each case, we check for the map file first
 and only re-create it if necessary.
 
 */

/* Fields in entries.idx */

#define PDBentries_raw_file           "entries.idx"
#define PDBentries_mmap_file       "PDBentries.dmp"
#define PDBentries_numfields                      8
#define PDBentries_skip_lines                     2
#define PDBentries_sep_char                     '\t'
#define PDBentries_id                             0
#define PDBentries_head                           1
#define PDBentries_acc                            2
#define PDBentries_cmpd                           3
#define PDBentries_src                            4
#define PDBentries_aut                            5
#define PDBentries_res                            6
#define PDBentries_exp                            7

/* Fields in PDBcelldatabase.tsv */

#define PDBcells_raw_file     "PDBcelldatabase.tsv"
#define PDBcells_mmap_file           "PDBcells.dmp"
#define PDBcells_numfields                        9
#define PDBcells_skip_lines                       1
#define PDBcells_sep_char                       '\t'
#define PDBcells_id                               0
#define PDBcells_cell_length_a                    1
#define PDBcells_cell_length_b                    2
#define PDBcells_cell_length_c                    3
#define PDBcells_cell_angle_alpha                 4
#define PDBcells_cell_angle_beta                  5
#define PDBcells_cell_angle_gamma                 6
#define PDBcells_space_group                      7
#define PDBcells_Z                                8

/* Fields in cod.tsv */

#define CODentries_raw_file               "cod.tsv"
#define CODentries_mmap_file       "CODentries.dmp"
#define CODentries_numfields                     15
#define CODentries_skip_lines                     0
#define CODentries_sep_char                     '\t'
#define CODentries_id                             0
#define CODentries_cell_length_a                  1
#define CODentries_cell_length_b                  2
#define CODentries_cell_length_c                  3
#define CODentries_cell_angle_alpha               4
#define CODentries_cell_angle_beta                5
#define CODentries_cell_angle_gamma               6
#define CODentries_symmetry_space_group_name_H_M  7
#define CODentries_cell_formula_units_Z           8
#define CODentries_publ_section_title             9
#define CODentries_publ_author_name              10
#define CODentries_chemical_formula_sum          11   /* Use for head */
#define CODentries_chemical_formula_weight       12
#define CODentries_chemical_name_common          13   /* combine with next for compound */
#define CODentries_chemical_name_systematic      14

/* Fields in CSDcelldatabase.tsv */

#define CSDcells_raw_file     "CSDcelldatabase.tsv"
#define CSDcells_mmap_file    "CSDcelldatabase.dmp"
#define CSDcells_sep_char                      '\t'
#define CSDcells_numfields                       16
#define CSDcells_skip_lines                       1
#define CSDcells_refcode                          0
#define CSDcells_cell_length_a                    1
#define CSDcells_cell_length_b                    2
#define CSDcells_cell_length_c                    3
#define CSDcells_cell_angle_alpha                 4
#define CSDcells_cell_angle_beta                  5
#define CSDcells_cell_angle_gamma                 6
#define CSDcells_space_group                      7
#define CSDcells_centring                         8
#define CSDcells_cell_red_length_a                9
#define CSDcells_cell_red_length_b               10
#define CSDcells_cell_red_length_c               11
#define CSDcells_cell_red_angle_alpha            12
#define CSDcells_cell_red_angle_beta             13
#define CSDcells_cell_red_angle_gamma            14
#define CSDcells_refcode_family                  15


#ifdef __cplusplus
    
}
#endif




#endif /* sauc_psm_files_create_h */
