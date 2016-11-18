//
//  sauc_psm_files_create.c
//  
//
//  Created by Herbert J. Bernstein on 9/18/16.
//
//

/**********************************************************************
 *  sauc_psm_files_create.c                                           *
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



#include "sauc_psm_files_create.h"
size_t ii;
PSM_localpsm_handle localpsm_handle;


/* Make an mmap file from a PDB, COD or CSD text file */

//*****************************************************************************
int make_psmfile(const char * mmapfilename, const char* textfilename,
                 size_t numhash, size_t * fieldnumbers,
                 size_t skip_lines, char sepchar, int CSDflag)
{
    FILE * infile;
    char * nextline;
    size_t linelen;
    char buffer [8200];
    size_t linecount;
    
    if ((localpsm_handle = PSM_mmap_file(mmapfilename))) {
        
        return 0;

    }
    
    
    localpsm_handle = PSM_localpsm_handle_create_wh(numhash,
                                                    fieldnumbers,
                                                    sepchar );
    infile = fopen(textfilename,"rb");
    
    for (ii = 0; ii < skip_lines; ii++) {
        nextline = fgetln(infile,&linelen);
    }
    linecount = 0;
    while ((nextline = fgetln(infile,&linelen))) {
        linecount++;
        if (linecount==1) {
            size_t llen;
            llen = linelen;
            if (linelen > 8191) llen = 8191;
            strncpy(buffer,nextline,llen);
            buffer[llen++] = '\0';
            fprintf(stdout,"first line: %s\n",buffer);
        }
        if (!CSDflag) {
            PSM_addstrn(localpsm_handle,nextline,linelen);
        } else {
            /* for CSD add an extra copy of the first 6 characters
             of the refcode as a family ID */
            size_t llen;
            llen = linelen;
            if (linelen > 1023) llen = 1023;
            strncpy(buffer,nextline,llen);
            buffer[llen++] = sepchar;
            for (ii = 0; ii < 6 && buffer[ii]!=sepchar && buffer[ii]!=0; ii++) {
                buffer[llen++] = buffer[ii];
            }
            buffer[llen] = '\0';
            PSM_addstrn(localpsm_handle,buffer,llen);
        }
    }
    
    fclose(infile);
    
    
    if (PSM_write_file(localpsm_handle,mmapfilename)) {
        
        return 1;
        
    }
    
    PSM_localpsm_handle_free(&localpsm_handle);
    fprintf(stdout,"%ld lines processed\n",(long)linecount);
    return 0;
    
}


int main(int argc, char** argv) {
    
    size_t PDBcfieldnumbers[2] = {PDBcells_id+1,PDBcells_numfields};
    size_t PDBefieldnumbers[2] = {PDBentries_id+1,PDBentries_head+1,PDBentries_numfields};
    size_t CSDfieldnumbers[2] = {CSDcells_refcode+1,
        CSDcells_refcode_family+1};
    size_t CODfieldnumbers[3] = {CODentries_id+1,CODentries_chemical_formula_sum+1,CODentries_numfields};
    
    
    if (argc < 2) {
        
        fprintf(stderr,"Usage:  sauc_psm_files_create [--help|PDB|COD|CSD]\n");
        
    }
    
    for (ii = 1; ii < argc; ii++) {
        
        if (!strcmp(argv[ii],"--help")) {
            
            fprintf(stderr,"Usage:  sauc_psm_files_create [--help|PDB|COD|CSD]\n");
        }  else if (!strcmp(argv[ii],"PDB")){
            
            if (make_psmfile(PDBentries_mmap_file, PDBentries_raw_file,
                             2, PDBefieldnumbers,
                             PDBentries_skip_lines, PDBentries_sep_char, 0)) {
                fprintf(stderr,"sauc_psm_files_create failed to establish PDBentries mmap");
                return 1;
            }
            
            if (make_psmfile(PDBcells_mmap_file, PDBcells_raw_file,
                             2, PDBcfieldnumbers,
                             PDBcells_skip_lines, PDBcells_sep_char, 0)) {
                fprintf(stderr,"sauc_psm_files_create failed to establish PDBcells mmap");
                return 1;
            }
        }  else if (!strcmp(argv[ii],"CSD")){
            
            if (make_psmfile(CSDcells_mmap_file, CSDcells_raw_file,
                             2, CSDfieldnumbers,
                             CSDcells_skip_lines, CSDcells_sep_char, 1)) {
                fprintf(stderr,"sauc_psm_files_create failed to establish CSDcells mmap");
                return 1;
            }
            
        }  else if (!strcmp(argv[ii],"COD")){
            
            if (make_psmfile(CODentries_mmap_file, CODentries_raw_file,
                             3, CODfieldnumbers,
                             CODentries_skip_lines, CODentries_sep_char, 0)) {
                fprintf(stderr,"sauc_psm_files_create failed to establish CODentries mmap");
                return 1;
            }
            
        }  else {
            
            fprintf(stderr,"Usage:  sauc_psm_files_create [--help|PDB|COD|CSD]\n");
            
        }
        
        
        
        
    }
    
    return 0;
    
    
}


