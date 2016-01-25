/**********************************************************************
 *  pststrmgr.h                                                       *
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


#ifndef pststrmgr_h
#define pststrmgr_h

#ifdef __cplusplus

extern "C" {
    
#endif
    
    
#include <sys/types.h>
    
    /*  There are two related but distinct data structurs used
     
     The persistent file structure is
     
     char 0-14    :char[14] "#PSM V. 1.0.0" followed by a 0 character
     char 15   :unsigned char[0]: sizeof(size_t)
     char 16-16+sizeof(size_t)-1:  size_t byte-order signature
     char 16+sizeof(size_t) - 39: reserved for future use
     char 40 - 40+sizeof(size_t)-1:  size_t index length
     char 40+sizeof(size_t) - 55: reserved for future use
     char 56 - 56+sizeof(size_t)-1:  size_t character buffer length
     char 56+sizeof(size_t) - 511: reserved for future use
     
     char 512 ff -- the character buffer, followed by the string index
     
     */
    
#define PSM_64BIT_BOSIGNATURE 0x0706050403020100L
#define PSM_32BIT_BOSIGNATURE 0x03020100L
#define PSM_FILE_SIGNATURE "#PSM V. 1.0.0"
    
    typedef union {
        char     char_version[16];
        size_t   size_t_version;
    } padded_size_t;
    
    typedef struct {
        char            idstring[14];
        unsigned char   sosize_t;
        padded_size_t   boflag;
        padded_size_t   index_len;
        padded_size_t   chars_len;
    } PSM_header;
    
    typedef struct {
        size_t length;      // The length of the string in bytes;
        size_t offset;      // The 0-based offset to the start of the string;
    } PSM_string;
    
    typedef struct {
        size_t    str_index_cap;    /* the maximum number of strings */
        size_t    str_index_len;    /* the actual number of strings */
        PSM_string * str_index;     /* pointer to the string index array */
        size_t    chars_cap;        /* the maximum number of characters in all strings*/
        size_t    chars_len;        /* the actual number of characters in all strings*/
        char    * chars;            /* the array of characters */
    } PSM_localpsm_handle_struct;
    
    typedef PSM_localpsm_handle_struct * PSM_localpsm_handle;
    
    
    typedef PSM_string * PSM_string_handle;
    
    
    /* create a local persistent string manager handle */
    
    PSM_localpsm_handle PSM_localpsm_handle_create( void );
    
    /* free a local persistent string manager handle */
    
    void PSM_localpsm_handle_free (PSM_localpsm_handle * handle);
    
    /* Get the index capacity of a local persistent string manager */
    
    size_t PSM_localpsm_get_string_index_capacity (PSM_localpsm_handle  handle);
    
    /* Get the index size of a local persistent string manager */
    
    size_t PSM_localpsm_get_string_index_len (PSM_localpsm_handle  handle);
    
    /* Get the character buffer capacity of a local persistent string manager */
    
    size_t PSM_localpsm_get_character_buffer_capacity (PSM_localpsm_handle  handle);
    
    /* Get the character buffer size of a local persistent string manager */
    
    size_t PSM_localpsm_get_character_buffer_len (PSM_localpsm_handle  handle) ;
    /* Set the index capacity of the local persistent string manager
     Returns the new capacity if set, 0 if not.
     You must release entries first if you wish to reduce the capacity
     below the length of entries array.  A request for a capacity less
     than the length will simply trim the capacity to the length.
     */
    
    size_t PSM_localpsm_set_string_index_capacity (PSM_localpsm_handle handle,
                                                   const size_t capacity);
    
    /* Set the character buffer capacity of the local persistent string manager
     Returns the new capacity if set, 0 if not.
     You must release entries first if you wish to reduce the capacity
     below the length of the character buffer.  A request for a capacity less
     than the length will simply trim the capacity to the length.
     */
    
    
    size_t PSM_localpsm_set_character_buffer_capacity (PSM_localpsm_handle handle,
                                                       const size_t capacity);
    
    /* Set the character buffer length of the local persistent string manager
     You may set the length to a lower value than the current length.
     You may not set it to a greater length
     */
    
    size_t PSM_localpsm_set_character_buffer_length (PSM_localpsm_handle handle,
                                                     const size_t length);
    
    /* Set the index length of the local persistent string manager
     You may set the length to a lower value than the current length
     You may not set it to a greater length */
    
    size_t PSM_localpsm_set_string_index_length (PSM_localpsm_handle handle,
                                                 const size_t length);
    
    
    /* Add a string to a persistent string
     Returns either a handle to the newly created PSM_string
     or NULL for failure */
    
    PSM_string_handle PSM_addstr(PSM_localpsm_handle handle, char * str);
    
    /* get a string from a persistent string
     Returns a newly allocated string that must be freed by the user
     */
    
    char * PSM_getstr(PSM_localpsm_handle handle, size_t offset);
    
    int PSM_write_file(PSM_localpsm_handle handle,const char * filepath);
    
    PSM_localpsm_handle PSM_mmap_file(const char * filepath);
    
#ifdef __cplusplus
    
}
#endif

#endif /* pststrmgr_h */
