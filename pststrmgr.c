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
 * ALTERNATIVELY YOU MAY REDISTRIBUTE THE pststrmgr API UNDER THE     *
 * TERMS OF THE LGPL                                                  *
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
    
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "pststrmgr.h"
    
    
        
    
    /* create a local persistent string manager handle with hashtables */
    
    PSM_localpsm_handle PSM_localpsm_handle_create_wh(size_t numhash,
                                                      size_t * fieldnumbers,
                                                      char split_char) {
        
        PSM_localpsm_handle localpsm_handle;
        
        localpsm_handle
        = (PSM_localpsm_handle)malloc(sizeof(PSM_localpsm_handle_struct));
        if (!localpsm_handle) return localpsm_handle;
        localpsm_handle->str_index_cap
        = localpsm_handle->str_index_len
        = localpsm_handle->chars_cap
        = localpsm_handle->chars_len
        = 0;
        localpsm_handle->str_index = (PSM_string *) NULL;
        localpsm_handle->chars = (char *) NULL;
        localpsm_handle->numhashtables = numhash;
        localpsm_handle->maxfieldno = 0;
        if (numhash == 0) {
            localpsm_handle->fieldnumbers = NULL;
            localpsm_handle->hashhead = NULL;
            localpsm_handle->hashlink = NULL;
            localpsm_handle->hashvalue = NULL;
            localpsm_handle->split_str = NULL;
            localpsm_handle->split_char = '\0';
        } else {
            size_t table;
            
            for (table = 0; table < numhash; table++) {
                if (fieldnumbers[table] > localpsm_handle->maxfieldno)
                    localpsm_handle->maxfieldno = fieldnumbers[table];
            }
            localpsm_handle->fieldnumbers
            = (size_t *)malloc(numhash*sizeof(size_t));
            if (localpsm_handle->maxfieldno > 0) {
                localpsm_handle->split_str
                = (PSM_string *)malloc((localpsm_handle->maxfieldno)*sizeof(PSM_string));
            }
            localpsm_handle->hashhead
            = (ssize_t *)malloc(numhash*PSM_HASHTABLE_SIZE*sizeof(ssize_t));
            if (!localpsm_handle->fieldnumbers
                ||!localpsm_handle->hashhead) {
                free (localpsm_handle);
                return NULL;
            }
            localpsm_handle->split_char = split_char;
            memset(localpsm_handle->hashhead,
                   0xFF,
                   numhash*PSM_HASHTABLE_SIZE*sizeof(ssize_t));
            memmove(localpsm_handle->fieldnumbers,
                    fieldnumbers,
                    numhash*sizeof(size_t));
            
            localpsm_handle->hashlink = NULL;
            localpsm_handle->hashvalue = NULL;
        }
        return localpsm_handle;
    }
    
    /* create a local persistent string manager handle w/o hashtables */
    
    PSM_localpsm_handle PSM_localpsm_handle_create( void ) {
        
        return PSM_localpsm_handle_create_wh( 0, NULL, ' ');
        
    }
    
    /* free a local persistent string manager handle */
    
    void PSM_localpsm_handle_free (PSM_localpsm_handle * handle) {
        
        if (!handle || !(*handle)) return;
        if ((*handle)->str_index) free((void *)((*handle)->str_index));
        if ((*handle)->chars) free ((void *)((*handle)->chars));
        if ((*handle)->hashhead) free ((void *)((*handle)->hashhead));
        if ((*handle)->hashlink) free ((void *)((*handle)->hashlink));
        if ((*handle)->hashvalue) free ((void *)((*handle)->hashvalue));
        if ((*handle)->fieldnumbers) free ((void *)((*handle)->fieldnumbers));
        if ((*handle)->split_str) free ((void *)((*handle)->split_str));
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
     
     The hash tables are extended in parallel
     */
    
    size_t PSM_localpsm_set_string_index_capacity (PSM_localpsm_handle handle,
                                                   const size_t capacity) {
        
        PSM_string * tempindex;
        
        ssize_t * temphashlink;
        
        ssize_t * temphashvalue;
        
        size_t cap = capacity;
        
        size_t oldcap = handle->str_index_cap;
        
        size_t ii;
        
        if (!handle) return 0;
        
        if (cap < handle->str_index_len) cap = handle->str_index_len;
        
        if (cap == oldcap) return cap;
        
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
        
        
        if (handle->numhashtables > 0) {
            
            if (cap) {
                temphashvalue = (ssize_t *)malloc((handle->numhashtables)*cap*sizeof(ssize_t));
                if (!temphashvalue) return 0;
                temphashlink = (ssize_t *)malloc((handle->numhashtables)*cap*sizeof(ssize_t));
                if (!temphashlink) return 0;
                
                if (handle->str_index_len) {
                    if (handle->str_index_len) {
                        for (ii = 0; ii <  handle->numhashtables; ii++ ) {
                            memmove(temphashvalue+ii*cap,
                                    handle->hashvalue+ii*oldcap,
                                    sizeof(ssize_t)*(handle->str_index_len));
                            memmove(temphashlink+ii*cap,
                                    handle->hashlink+ii*oldcap,
                                    sizeof(ssize_t)*(handle->str_index_len));
                        }
                    }
                }
                if (handle->hashvalue) free(handle->hashvalue);
                if (handle->hashlink) free(handle->hashlink);
            } else {
                temphashvalue = NULL;
                temphashlink = NULL;
                if (handle->hashvalue) free(handle->hashvalue);
                if (handle->hashlink) free(handle->hashlink);
            }
            handle->hashvalue = temphashvalue;
            handle->hashlink = temphashlink;
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
        
        size_t ii,jj;
        ssize_t hashlink;
        
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
        
        if (handle->numhashtables == 0
            || !handle->hashhead
            || !handle->hashlink) return length;
        
        /* We need to trim the hashhead entries to only point to
         valid index entries, and we assume the 2s-complement
         equivalence of 0xFFF... to -1.  We also assume the
         chain of links _always_ run backwards from highest to
         lowest. */
        
        if (length == 0) {
            memset(handle->hashhead,
                   -1,
                   (handle->numhashtables)*PSM_HASHTABLE_SIZE*sizeof(ssize_t));
        } else {
            for (ii = 0; ii < handle->numhashtables; ii++) {
                for (jj = 0; jj < PSM_HASHTABLE_SIZE; jj++) {
                    hashlink = PSM_hashhead_entry(handle,ii,jj);
                    while (hashlink >= (ssize_t)length) {
                        PSM_hashhead_entry(handle,ii,jj)
                        = PSM_hashlink_entry(handle,ii,hashlink);
                        hashlink = PSM_hashhead_entry(handle,ii,jj);
                    }
                }
            }
        }
        
        return length;
        
    }
    
    /* Splits a PSM_string into an array of nfields PSM_strings on the basis
     of a separator character c.  Returns split_string on success, NULL on
     failure
     */
    
    PSM_string * PSM_split_psm_string(PSM_localpsm_handle handle,
                                      PSM_string* split_string,
                                      size_t nfields,
                                      PSM_string str,
                                      char c) {
        
        size_t ifield, ipos, base_offset;
        char cfirst, clast;
        char xc;
        int qcount;
        
        if (!handle
            || !split_string
            || !nfields
            || !(handle->chars_len))
            return NULL;
        
        ipos = base_offset = str.offset;
        
        if (nfields > handle->maxfieldno)
            nfields = handle->maxfieldno;
        
        for (ifield = 0; ifield < nfields; ifield++) {
            split_string[ifield].offset = base_offset;
            split_string[ifield].length = 0;
            
            /* For a blank separator, trim leading
             blanks, tabs or newlines */
            
            if (c==' ') {
                while (ipos < str.length+str.offset
                       && ((xc=handle->chars[ipos] == ' ')
                           || xc == '\t'
                           || xc == '\n')) {
                           ipos++;
                       }
                split_string[ifield].offset = ipos;
                
                /* For a tab separator, trim leading newlines */
            } else if (c=='\t') {
                while (ipos < str.length+str.offset
                       && ((xc=handle->chars[ipos]) == '\n')) {
                    ipos++;
                }
                split_string[ifield].offset = ipos;
            }
            
            qcount = 0;
            cfirst = clast = handle->chars[ipos];
            if (cfirst == '\'' || cfirst == '"') qcount = 2;
            while (ipos < str.length+str.offset
                   && (handle->chars[ipos] != c||qcount > 0)) {
                clast = handle->chars[ipos];
                if(qcount > 0 && clast == cfirst && (cfirst == '\'' || cfirst == '"')) {
                    qcount--;
                    if (qcount == 0) {
                        ipos++;
                        (split_string[ifield].length)++;
                        while (ipos < str.length+str.offset
                               && (handle->chars[ipos] == ' '
                                   || handle->chars[ipos] == '\t'
                                   || handle->chars[ipos] == '\n')) {
                                   if (ipos >= str.length+str.offset
                                       || handle->chars[ipos] == c) break;
                                   ipos++;
                               }
                        if (ipos >= str.length+str.offset
                            || handle->chars[ipos] == c) break;
                    }
                }
                ipos++;
                (split_string[ifield].length)++;
                
            }
            
            /* Strip one layer of quotes */
            if (split_string[ifield].length > 1 &&
                cfirst == clast &&
                (cfirst == '\'' || cfirst=='"')) {
                split_string[ifield].offset++;
                split_string[ifield].length -= 2;
            }
            if (c == ' ' ) {
                while (split_string[ifield].length > 0
                       && ((xc=handle->chars[split_string[ifield].offset+split_string[ifield].length -1] == ' ')
                           || xc == '\t'
                           || xc == '\n')) {
                           split_string[ifield].length--;
                       }
            } else  if (c == ' ' ) {
                while (split_string[ifield].length > 0
                       && ((xc=handle->chars[split_string[ifield].offset+split_string[ifield].length -1] == ' ')
                           || xc == '\t'
                           || xc == '\n')) {
                           split_string[ifield].length--;
                       }
            }
            
            
            if (ipos < str.length+str.offset
                && handle->chars[ipos] == c) ipos++;
            base_offset = ipos;
        }
        
        return split_string;
    }
    
    /* Add a string to a persistent string
     Returns either a handle to the newly created PSM_string
     or NULL for failure */
    
    PSM_string_handle PSM_addstr(PSM_localpsm_handle handle, char * str) {
        
        if (!handle) return NULL;
        
        return PSM_addstrn(handle, str, strlen(str));
        
    }
    
    
    /* Add a length-limited string to a persistent string
     Returns either a handle to the newly created PSM_string
     or NULL for failure */
    
    PSM_string_handle PSM_addstrn(PSM_localpsm_handle handle,
                                  char * str,
                                  size_t strsize) {
        
        size_t new_cap;
        
        size_t ii;
        
        char * pstr;
        
        if (!handle) return NULL;
        
        if (handle->str_index_len >= handle->str_index_cap) {
            
            new_cap = 1+2*(handle->str_index_cap);
            if (new_cap != PSM_localpsm_set_string_index_capacity(handle,new_cap)) return NULL;
            
        }
        
        if (handle->chars_len+strsize >= handle->chars_cap) {
            
            new_cap = 511+strsize + 2*handle->chars_cap;
            
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
        
        if (handle-> numhashtables > 0
            && handle->split_str
            && handle->hashhead
            && handle->hashlink
            && handle->hashvalue
            && handle->fieldnumbers
            && PSM_split_psm_string(handle,
                                    handle->split_str,
                                    handle->maxfieldno,
                                    *((handle->str_index)+(handle->str_index_len)-1),
                                    handle->split_char)) {
                size_t table;
                ssize_t hashlink, hashvalue;
                
                /* For each hash table add the latest link as the last in the chain
                 so that the pruning code when reducing sizes works */
                
                for (table=0; table < handle->numhashtables; table++) {
                    if (handle->fieldnumbers[table] == 0) {
                        hashvalue = PSM_str_hashvalue(handle->chars+handle->str_index[handle->str_index_len-1].offset,
                                                      handle->str_index[handle->str_index_len-1].length);
                    } else {
                        hashvalue = PSM_str_hashvalue((handle->chars)
                                                      +(handle->split_str)[(handle->fieldnumbers[table]-1)].offset,
                                                      (handle->split_str)[(handle->fieldnumbers[table]-1)].length);
                    }
                    PSM_hashvalue_entry(handle,table,(handle->str_index_len)-1)=hashvalue;
                    hashlink = PSM_hashhead_entry(handle,table,hashvalue);
                    PSM_hashhead_entry(handle,table,hashvalue) = (handle->str_index_len)-1;
                    PSM_hashlink_entry(handle,table,(handle->str_index_len)-1) = hashlink;
                    if ((long)(handle->str_index_len) <= 10L ) {
                        fprintf(stderr,"table %ld item %ld hashvalue %ld key '",
                                (long)table, (long)(handle->str_index_len-1),
                                (long)hashvalue);
                        for (ii=0; ii < (handle->split_str)[(handle->fieldnumbers[table]-1)].length; ii++) {
                            fprintf(stderr,"%c",*((handle->chars)
                                                  +(handle->split_str)[(handle->fieldnumbers[table]-1)].offset+ii));
                        }
                        fprintf(stderr,"'\n");
                        
                    }
                    
                }
                
            }
        
        
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

    
    /* get a string from a persistent string field
     Returns a newly allocated string that must be freed by the user
     */
    
    char * PSM_getstrfield(PSM_localpsm_handle handle, PSM_string field) {
        
        char * string;
        
        char * pstr;
        
        size_t ii;
        
        if ( handle == NULL ) return NULL;
        
        pstr = string = (char *) malloc(field.length+1);
        
        if (!string) return string;
        
        for (ii=0; ii < field.length; ii++)
        {         *pstr++ = *(handle->chars+field.offset+ii);
        }
        
        *pstr++ = '\0';
        
        return string;
        
    }
    
    /* get a string from a persistent string by key from a particular
     hash table
     
     Returns a newly allocated string that must be freed by the user
     */
    
    char * PSM_getstr_by_key(PSM_localpsm_handle handle, const char * key, size_t table) {
        
        ssize_t hashlink;
        
        hashlink = PSM_getstrindex_by_key(handle, key, table);
        
        if (hashlink < (ssize_t)handle->str_index_len && hashlink >= 0) {
            
            return PSM_getstr(handle,hashlink);
            
        }
        
        return NULL;
    }
    
    
    /* get the index of a persistent string by key from a particular
     hash table
     
     */
    
    ssize_t PSM_getstrindex_by_key(PSM_localpsm_handle handle, const char * key, size_t table) {
        
        ssize_t hashvalue, hashlink;
        
        size_t cmplen, maxlen;
        
        PSM_string split_str[handle->maxfieldno];
        
        PSM_string * selected;
        
        if (!handle || !key || table >= handle->numhashtables) return (ssize_t)-1L;
        maxlen = strnlen(key,PSM_HASHTABLE_MAXSTR);
        
        hashvalue = PSM_str_hashvalue(key,maxlen);
        
        hashlink = PSM_hashhead_entry(handle,table,hashvalue);
        
        
        while (hashlink < (ssize_t)handle->str_index_len
               && hashlink >= 0
               && (handle->fieldnumbers[table] == 0
                   || PSM_split_psm_string(handle,
                                           split_str,
                                           handle->maxfieldno,
                                           handle->str_index[hashlink],
                                           handle->split_char))) {
                       
                       if (handle->fieldnumbers[table] == 0) {
                           
                           selected = handle->str_index + hashlink;
                           
                       } else {
                           
                           selected = split_str + handle->fieldnumbers[table] -1;
                           
                       }
                       
                       cmplen=selected->length;
                       
                       if (cmplen == maxlen
                           && !strncmp(key,
                                       (handle->chars)+selected->offset,
                                       maxlen) ){
                               
                               return hashlink;
                               
                           }
                       
                       hashlink =PSM_hashlink_entry(handle,table,hashlink);
                       
                   }
        
        return (ssize_t)-1L;
    }
    
    
    int PSM_write_file(PSM_localpsm_handle handle, const char * filepath) {
        
        FILE * fstream;
        
        PSM_header header;
        
        unsigned char pad[512-sizeof(PSM_header)];
        
        unsigned char charspad[8192];
        
        memset((void *)(&header),0,sizeof(PSM_header));
        
        memset((void *)(&pad),0,512-sizeof(PSM_header));
        
        memset((void *)(&charspad),0,8192);

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
        
        header.numhashtables.size_t_version = handle->numhashtables;
        
        header.maxfieldno.size_t_version = handle->maxfieldno;
        
        header.split_char = handle->split_char;
        
        if (!(fstream=fopen(filepath,"wb"))) return -1;
        
        /* write the PSM_header  with pad to 512 characters*/
        
        fwrite(&header,sizeof(PSM_header),1,fstream);
        
        fwrite(&pad,512-sizeof(PSM_header),1,fstream);
        
        /* write the variable length data in the order
         str_index
         fieldnumbers
         hashhead
         hashlink
         hashvalue
         chars */
        
        fwrite(handle->str_index,(handle->str_index_len)*sizeof(PSM_string),1,fstream);
        
        fwrite(handle->fieldnumbers,(handle->numhashtables)*sizeof(size_t),1,fstream);
        
        if (handle->numhashtables) {
            fwrite(handle->hashhead,
                   (handle->numhashtables)*PSM_HASHTABLE_SIZE*sizeof(ssize_t),
                   1,fstream);
            
            if (handle->hashlink && handle->str_index_len) {
                fwrite(handle->hashlink,
                       (handle->numhashtables)*(handle->str_index_len)*sizeof(ssize_t),
                       1,fstream);
            }
            
            if (handle->hashvalue && handle->str_index_len) {
                fwrite(handle->hashvalue,
                       (handle->numhashtables)*(handle->str_index_len)*sizeof(ssize_t),
                       1,fstream);
            }
            
        }
        
        fwrite(handle->chars,handle->chars_len,1,fstream);
        
        fwrite(&charspad,8192,1,fstream);
        
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
        = header->index_len.size_t_version;
        handle->chars_cap = handle->chars_len
        = header->chars_len.size_t_version;
        handle->numhashtables = header->numhashtables.size_t_version;
        handle->maxfieldno = header->maxfieldno.size_t_version;
        handle->split_char = (char)(0xff&header->split_char);
        
        msync(header,sizeof(PSM_header),MS_SYNC);
        munmap(header,sizeof(PSM_header));
        
        close(fd);
        
        fd = open(filepath, O_RDONLY);
        
        if (fd < 0) {
            
            PSM_localpsm_handle_free(&handle);
            
            return NULL;
            
        }
        
        while (1) {
            handle->str_index = (PSM_string *)mmap(0,
                                                   (handle->str_index_len)*sizeof(PSM_string)
                                                   + (handle->numhashtables)*sizeof(size_t)
                                                   + (handle->numhashtables)*PSM_HASHTABLE_SIZE*sizeof(ssize_t)
                                                   + (handle->numhashtables)*(handle->str_index_len)*sizeof(ssize_t)
                                                   + (handle->numhashtables)*(handle->str_index_len)*sizeof(ssize_t)
                                                   + handle->chars_cap+8192,
                                         PROT_READ,
                                         MAP_SHARED | MAP_FILE, fd, 0);
            
            if (handle->str_index == MAP_FAILED) {
                
                handle->str_index = NULL;
                
                PSM_localpsm_handle_free(&handle);
                
                if (errno != EBUSY)return NULL;
                
                sleep(1);
                
                continue;
                
            } else break;
        }
        
        handle->str_index = (PSM_string *)(((char *)(handle->str_index))+512);
        handle->fieldnumbers = (size_t *)(handle->str_index+handle->str_index_len);
        if (handle->numhashtables > 0) {
            handle->hashhead = (ssize_t *)(handle->fieldnumbers
                                           +handle->numhashtables);
            handle->hashlink = (ssize_t *)(handle->hashhead
                                           +PSM_HASHTABLE_SIZE
                                           *(handle->numhashtables));
            handle->hashvalue = (ssize_t *)(handle->hashlink
                                            +(handle->str_index_len)
                                            *(handle->numhashtables));
            handle->chars = (char *)(handle->hashvalue
                                     +(handle->str_index_len)
                                     *(handle->numhashtables));
            if (handle->str_index_len == 0) handle->hashvalue = 0;
        } else {
            handle->hashhead = handle->hashlink = handle->hashvalue = NULL;
            handle->chars = (char *)(handle->fieldnumbers
                                     +handle->numhashtables);
        }
        
        return handle;
        
    }
    
#ifdef __cplusplus
    
}
#endif


