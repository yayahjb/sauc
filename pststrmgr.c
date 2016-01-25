/**********************************************************************
 *  pststrmgr.c                                                       *
 *  Persistent String Manager                                         *
 *  Copyright 2016 Herbert J. Bernstein, all rights reserved          *
 *                                                                    *
 *  Created by Herbert J. Bernstein on 1/22/16.                       *
 **********************************************************************/

/**********************************************************************
 *                                                                    *
 * YOU MAY REDISTRIBUTE THE pststrmgr PACKAGE UNDER THE TERMS OF THE  *
 * GPL                                                                *
 *                                                                    *
 * ALTERNATIVELY YOU MAY REDISTRIBUTE THE CBFLIB API UNDER THE TERMS  *
 * OF THE LGPL                                                        *
 *                                                                    *
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


#ifdef __cplusplus

extern "C" {
    
#endif
    
#include "pststrmgr.h"
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
    
    /* create a local persistent string manager handle */
    
    PSM_localpsm_handle PSM_localpsm_handle_create( void ) {
        
        PSM_localpsm_handle localpsm_handle;
        
        localpsm_handle = (PSM_localpsm_handle)
        malloc(sizeof(PSM_localpsm_handle_struct));
        if (!localpsm_handle) return localpsm_handle;
        localpsm_handle->str_index_cap
        = localpsm_handle->str_index_len
        = localpsm_handle->chars_cap
        = localpsm_handle->chars_len
        = 0;
        localpsm_handle->str_index = (PSM_string *) NULL;
        localpsm_handle->chars = (char *) NULL;
        return localpsm_handle;
    }
    
    /* free a local persistent string manager handle */
    
    void PSM_localpsm_handle_free (PSM_localpsm_handle * handle) {
        
        if (!handle || !(*handle)) return;
        if ((*handle)->str_index) free((void *)((*handle)->str_index));
        if ((*handle)->chars) free ((void *)((*handle)->chars));
        free ((void *)(*handle));
        *handle = NULL;
    }
    
    /* Get the index capacity of a local persistent string manager */
    
    size_t PSM_localpsm_get_string_index_capacity (PSM_localpsm_handle  handle) {
        
        if (!handle) return 0;
        return handle->str_index_cap;
    }
    
    /* Get the index size of a local persistent string manager */
    
    size_t PSM_localpsm_get_string_index_len (PSM_localpsm_handle  handle) {
        
        if (!handle) return 0;
        return handle->str_index_len;
    }
    
    /* Get the character buffer capacity of a local persistent string manager */
    
    size_t PSM_localpsm_get_character_buffer_capacity (PSM_localpsm_handle  handle) {
        
        if (!handle) return 0;
        return handle->chars_cap;
    }
    
    /* Get the character buffer size of a local persistent string manager */
    
    size_t PSM_localpsm_get_character_buffer_len (PSM_localpsm_handle  handle) {
        
        if (!handle) return 0;
        return handle->chars_len;
    }
    
    /* Set the index capacity of the local persistent string manager
     Returns the new capacity if set, 0 if not.
     You must release entries first if you wish to reduce the capacity
     below the length of entries array.  A request for a capacity less
     than the length will simply trim the capacity to the length.
     */
    
    size_t PSM_localpsm_set_string_index_capacity (PSM_localpsm_handle handle,
                                                   const size_t capacity) {
        
        PSM_string * tempindex;
        
        size_t cap = capacity;
        
        if (!handle) return 0;
        
        if (cap < handle->str_index_len) cap = handle->str_index_len;
        
        if (cap == handle->str_index_cap) return handle->str_index_cap;
        
        if (cap) {
            tempindex = (PSM_string *)malloc(cap*sizeof(PSM_string));
            if (!tempindex) return 0;
            
            if (handle->str_index_len) {
                memmove((void *)tempindex, (void *) (handle->str_index),
                        (handle->str_index_len)*sizeof(PSM_string) );
            }
            if (handle->str_index) free(handle->str_index);
        } else {
            tempindex = NULL;
            if (handle->str_index) free(handle->str_index);
        }
        
        handle->str_index = tempindex;
        handle->str_index_cap = cap;
        
        return cap;
    }
    
    /* Set the character buffer capacity of the local persistent string manager
     Returns the new capacity if set, 0 if not.
     You must release entries first if you wish to reduce the capacity
     below the length of the character buffer.  A request for a capacity less
     than the length will simply trim the capacity to the length.
     */
    
    
    size_t PSM_localpsm_set_character_buffer_capacity (PSM_localpsm_handle handle,
                                                       const size_t capacity) {
        
        char * tempchars;
        
        size_t cap = capacity;
        
        if (!handle) return 0;
        
        if (cap < handle->chars_len) cap = handle->chars_len;
        
        if (cap == handle->chars_cap) return handle->chars_cap;
        
        if (cap) {
            /*fprintf(stderr,"changing chars_cap to %ld\n",(long)cap); */
            tempchars = (char *)malloc(cap*sizeof(char));
            if (!tempchars) return 0;
            
            if (handle->chars_len) {
                memmove((void *)tempchars, (void *) (handle->chars),
                        (handle->chars_len)*sizeof(char) );
            }
            if (handle->chars) free(handle->chars);
        } else {
            tempchars = NULL;
            if (handle->chars) free(handle->chars);
        }
        
        handle->chars = tempchars;
        handle->chars_cap = cap;
        
        return cap;
    }
    
    /* Set the character buffer length of the local persistent string manager
     You may set the length to a lower value than the current length.
     You may not set it to a greater length
     */
    
    size_t PSM_localpsm_set_character_buffer_length (PSM_localpsm_handle handle,
                                                     const size_t length) {
        
        size_t ilow, imid, ihigh;
        
        if (!handle || length > handle->chars_len) return 0;
        
        if (length == handle->chars_len) {
            
            return handle->chars_len;
            
        } else {
            /* The new length is less than the current character buffer length,
             index entries pointing to blocks ending over that length must
             be released */
            ilow = 0;
            ihigh = handle->str_index_len-1;
            imid = (ilow+ihigh+1)/2;
            
            while ((handle->str_index)[ilow].offset < length
                   && (handle->str_index)[ihigh].offset
                   + (handle->str_index)[ihigh].length >= length) {
                if (ilow == ihigh
                    || (handle->str_index)[ilow].offset
                    + (handle->str_index)[ilow].length >= length) {
                    if (ilow != PSM_localpsm_set_string_index_length(handle,ilow))
                        return 0;
                    break;
                }
                if ((handle->str_index)[imid].offset < length) {
                    ilow = imid;
                } else {
                    if ((handle->str_index)[ihigh].offset < length) {
                        if (ihigh != PSM_localpsm_set_string_index_length(handle,ihigh))
                            return 0;
                        break;
                    }
                    ihigh = (ihigh==imid)?imid-1:imid;
                }
                imid = (ilow+ihigh+1)/2;
            }
        }
        
        /* We now set the length to the length needed to hold the last index
         entry */
        
        if (handle->str_index_len > 0) {
            
            return(handle->chars_len = (handle->str_index)[handle->str_index_len-1].offset
                   + (handle->str_index)[handle->str_index_len-1].length);
        } else {
            return(handle->chars_len = 0);
        }
        
    }
    
    
    /* Set the index length of the local persistent string manager
     You may set the length to a lower value than the current length
     You may not set it to a greater length */
    
    size_t PSM_localpsm_set_string_index_length (PSM_localpsm_handle handle,
                                                 const size_t length) {
        
        if (!handle || length > handle->str_index_len) return 0;
        
        if (length == handle->str_index_len) {
            return handle->str_index_len;
        } else {
            
            if ( (handle->str_index[length]).offset
                != PSM_localpsm_set_character_buffer_length(handle,
                                                            (handle->str_index[length]).offset))
                return 0;
        }
        
        handle->str_index_len = length;
        
        return length;
    }
    
    
    /* Add a string to a persistent string
     Returns either a handle to the newly created PSM_string
     or NULL for failure */
    
    PSM_string_handle PSM_addstr(PSM_localpsm_handle handle, char * str) {
        
        size_t new_cap;
        
        size_t strsize;
        
        size_t ii;
        
        char * pstr;
        
        if (!handle) return NULL;
        
        strsize = strlen(str);
        
        if (handle->str_index_len >= handle->str_index_cap) {
            
            new_cap = 1+2*(handle->str_index_cap);
            if (new_cap != PSM_localpsm_set_string_index_capacity(handle,new_cap)) return NULL;
            
        }
        
        if (handle->chars_len+strsize >= handle->chars_cap) {
            
            new_cap = strsize + 2*handle->chars_cap;
            
            if (new_cap != PSM_localpsm_set_character_buffer_capacity(handle,new_cap)) return NULL;
            
        }
        
        pstr = str;
        for (ii=handle->chars_len; ii < handle->chars_len+strsize && *pstr; ii++) {
            (handle->chars)[ii] = *pstr++;
        }
        
        handle->str_index[handle->str_index_len].length = strsize;
        handle->str_index[handle->str_index_len].offset = handle->chars_len;
        (handle->chars_len)+= strsize;
        (handle->str_index_len)++;
        
        return (handle->str_index)+(handle->str_index_len)-1;
    }
    
    /* get a string from a persistent string
     Returns a newly allocated string that must be freed by the user
     */
    
    char * PSM_getstr(PSM_localpsm_handle handle, size_t offset) {
        
        char * string;
        
        char * pstr;
        
        size_t ii;
        
        if (!handle || offset >= handle->str_index_len) return NULL;
        
        pstr = string = (char *) malloc(handle->str_index[offset].length+1);
        
        if (!string) return string;
        
        for (ii=0; ii < handle->str_index[offset].length; ii++)
        {         *pstr++ = *(handle->chars+handle->str_index[offset].offset+ii);
        }
        
        *pstr++ = '\0';
        
        return string;
        
    }

    
    
    int PSM_write_file(PSM_localpsm_handle handle, const char * filepath) {
        
        FILE * fstream;
        
        size_t ii;
        
        PSM_header header;
        
        unsigned char pad[512-sizeof(PSM_header)];
        
        memset((void *)(&header),0,sizeof(PSM_header));
        
        memset((void *)(&pad),0,512-sizeof(PSM_header));
        
        if (sizeof(size_t) == 8) {
            header.boflag.size_t_version = PSM_64BIT_BOSIGNATURE;
        } else if (sizeof(size_t) == 4) {
            header.boflag.size_t_version = PSM_32BIT_BOSIGNATURE;
        } else {
            fprintf(stderr,"PSM error -- unsupported sizeof(size-t) \n");
            return -1;
        }
        
        strcpy(header.idstring,PSM_FILE_SIGNATURE);
        
        header.sosize_t = (unsigned char)sizeof(size_t);
        
        header.index_len.size_t_version = handle->str_index_len;
        
        header.chars_len.size_t_version = handle->chars_len;
        
        ii = 512 + handle->chars_len + handle->str_index_len;
        
        if (!(fstream=fopen(filepath,"wb"))) return -1;
        
        fwrite(&header,sizeof(PSM_header),1,fstream);
        
        fwrite(&pad,512-sizeof(PSM_header),1,fstream);
        
        fwrite(handle->chars,handle->chars_len,1,fstream);
        
        fwrite(handle->str_index,(handle->str_index_len)*sizeof(PSM_string),1,fstream);
        
        fclose(fstream);
        
        return 0;
        
    }
    
    PSM_localpsm_handle PSM_mmap_file(const char * filepath) {
        
        PSM_header * header;
        
        PSM_localpsm_handle handle;
        
        size_t file_size_t_size;
        
        int goodbo;
        
        int fd;
        
        if (!(handle = PSM_localpsm_handle_create())) return NULL;
        
        fd = open(filepath, O_RDONLY);
        
        if (fd < 0) {
            
            PSM_localpsm_handle_free(&handle);
            
            return NULL;
            
        }
        
        header = (PSM_header *)mmap(0, sizeof(PSM_header), PROT_READ,
                                    MAP_SHARED | MAP_FILE, fd, 0);
        
        if (header->idstring[13] != '\0' ||
            strcmp(header->idstring,PSM_FILE_SIGNATURE)) return NULL;
        
        file_size_t_size = header->sosize_t;
        
        if (file_size_t_size != sizeof(size_t)) return NULL;
        
        goodbo = 0;
        
        if ((sizeof(size_t) == 8 &&
             header->boflag.size_t_version == (size_t)PSM_64BIT_BOSIGNATURE)
            || (sizeof(size_t) == 4 &&
                header->boflag.size_t_version == (size_t)PSM_32BIT_BOSIGNATURE)) goodbo =1;
        
        if (!goodbo) {
            
            PSM_localpsm_handle_free(&handle);
            
            return NULL;
            
        }
        
        handle->str_index_cap = handle->str_index_len
        = header->index_len.size_t_version*sizeof(PSM_string);
        handle->chars_cap = handle->chars_len
        = header->chars_len.size_t_version;
        
        msync(header,sizeof(PSM_header),MS_SYNC);
        munmap(header,sizeof(PSM_header));
        
        close(fd);
        
        fd = open(filepath, O_RDONLY);
        
        if (fd < 0) {
            
            PSM_localpsm_handle_free(&handle);
            
            return NULL;
            
        }
        
        while (true) {
            handle->chars = (char *)mmap(0,
                                         handle->chars_cap+
                                         handle->str_index_cap*sizeof(PSM_string),
                                         PROT_READ,
                                         MAP_SHARED | MAP_FILE, fd, 0);
            
            if (handle->chars == MAP_FAILED) {
                
                handle->chars = NULL;
                
                PSM_localpsm_handle_free(&handle);
                
                if (errno != EBUSY)return NULL;
                
                sleep(1);
                
                continue;
                
            } else break;
        }
        
        handle->chars += 512;
        handle->str_index = (PSM_string *)(handle->chars+handle->chars_cap);
        
        return handle;
        
    }
    
#ifdef __cplusplus
    
}
#endif


