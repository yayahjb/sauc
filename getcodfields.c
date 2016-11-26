#include <stdio.h>
#include <string.h>
#include "pststrmgr.h"
FILE * in;
PSM_localpsm_handle psmstrs;
char * fgetln(FILE *stream, size_t *len);
size_t zincfields[5] = {1,2,3,4,5};
PSM_string str;
size_t iline;
char id1[1024];
char idc[1024];

void putstrn( char * src, size_t srclen) {
    size_t ii;
    char pchar;
    pchar =' ';
    for (ii = 0; ii < srclen; ii++) {
      if (src[ii] == '\\' && ii < srclen-1 && src[ii+1] =='n' ) {
        if (pchar != ' ') putchar(' ');
        putchar('/');
        putchar('/');
        pchar = '/';
        ii++;
      } else {
        putchar(src[ii]);
        pchar = src[ii];
      }
    }
}

int main (int argc, char ** argv) {

    psmstrs = PSM_localpsm_handle_create_wh(5,zincfields,'\t');
    if (argc < 2) {
        in = stdin;
    } else {
        in = fopen(argv[1],"r");
        if (!in) return -1;
    }

    iline = 0;
    while((psmstrs->chars = fgetln(in,&(psmstrs->chars_len)))){
      str.length =  psmstrs->chars_cap = psmstrs->chars_len;
      str.offset =  0;
      if (PSM_split_psm_string(psmstrs, psmstrs->split_str,
                            psmstrs->maxfieldno, str, '\t')) {
        if (iline == 0) {
          strncpy(id1,psmstrs->chars+psmstrs->split_str[0].offset,psmstrs->split_str[0].length>1023?1023:psmstrs->split_str[0].length);
          id1[1023] = '\0';
        }

        strncpy(idc,psmstrs->chars+psmstrs->split_str[0].offset,psmstrs->split_str[0].length>1023?1023:psmstrs->split_str[0].length);
        idc[1023] = '\0';
        if (strcmp(id1,idc) != 0) {
          fprintf (stderr,"getcodfields id mismatch '%s' '%s'\n",id1, idc);
        }

        if ( iline > 0 ) {

          if (psmstrs->split_str[2].length != 0
              && *(psmstrs->chars+psmstrs->split_str[2].offset)!= '0') {
                putchar(';');
          } else {
            putchar('\t');
          }
        }
        if  (iline > 0) {
          putstrn(psmstrs->chars+psmstrs->split_str[3].offset,
                  psmstrs->split_str[3].length);
        } else {
          fprintf(stdout,id1);
        }
        iline++;
      }
    }
    putchar('\n');
}
