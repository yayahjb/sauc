#include <iostream>
#include <string>
#include "pststrmgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char ** argv) {
    
    size_t ii;
    
    char buffer[80];
    
    char key[80];
    
    char* obuffer;
    
    size_t fieldnumbers[2] = {4,6};
    
    PSM_localpsm_handle psmin, psmin_wh, psmout, psmout_wh;
    
    psmin = PSM_localpsm_handle_create();
    
    psmin_wh = PSM_localpsm_handle_create_wh(2,fieldnumbers,' ');
    
    obuffer = NULL;

    for (ii=0; ii < 2000000L; ii++) {
        
        if (obuffer) free(obuffer);
        
        sprintf(buffer,std::string("Test string number %ld .......................................... %ld").c_str(), ii, 1999999L-ii);
        
        if (!PSM_addstr(psmin,buffer)) {
            
            std::cout << "Failed to add string " << ii << " to psmin" << std::endl;
            
            return -1;
        
        }
        
        if (!PSM_addstr(psmin_wh,buffer)) {
            
            std::cout << "Failed to add string " << ii << " to psmin_wh" << std::endl;
            
            return -1;
    }
    
    }
    
    
    /* Pre-mapping tests */
    
    std::cout << "Pre-mapping tests, no hash" << std::endl;
    
    for (ii=0; ii < 2000000L; ii++) {
        
        if (obuffer) free(obuffer);

        if (ii%20000 == 0) std::cout << "Pass: " << ii << std::endl;
        
        sprintf(buffer,std::string("Test string number %ld .......................................... %ld").c_str(), ii, 1999999L-ii);
        
        obuffer = PSM_getstr(psmin,ii);
        
        if (!obuffer || strcmp(buffer,obuffer)) {
            
            std::cout << "Failed to get string " << ii << " from psmout" << std::endl;
            
            std::cout << "Should be " << buffer << std::endl;
            
            if (obuffer) std::cout << "       is " << obuffer << std::endl;
            
        }
        
    }

    std::cout << "Pre-mapping tests, with hash" << std::endl;
    
    for (ii=0; ii < 2000000L; ii++) {

        if (obuffer) free(obuffer);
        
        if (ii%20000 == 0) std::cout << "Pass: " << ii << std::endl;
        
        sprintf(key,std::string("%ld").c_str(), (((1999999L-ii))*3)%(2000000L));
        
        sprintf(buffer,std::string("Test string number %ld .......................................... %ld").c_str(), ii, 1999999L-ii);

        obuffer = PSM_getstr(psmin_wh,ii);
        
        if (strcmp(buffer,obuffer)) {
            
            std::cout << "Failed to get string, no key " << ii << " from psmout_wh" << std::endl;
            
            std::cout << "Should be " << buffer << std::endl;
            
            std::cout << "       is " << obuffer << std::endl;
            
        }
        
        if (obuffer) free(obuffer);
        
        obuffer = PSM_getstr_by_key(psmin_wh, key, 0 );
        
        sprintf(buffer,std::string("Test string number %ld .......................................... %ld").c_str(), (((1999999L-ii))*3)%(2000000L), 1999999L-(((1999999L-ii))*3)%(2000000L) );
        
        if (!obuffer || strcmp(buffer,obuffer)) {
            
            std::cout << "Failed to get string by key 0 " << (((1999999L-ii))*3)%(2000000L) << " from psmout_wh" << std::endl;
            
            std::cout << "Should be " << buffer << std::endl;
            
            if (obuffer) std::cout << "       is " << obuffer << std::endl;
            
        }

        if (obuffer) free(obuffer);
        
        obuffer = PSM_getstr_by_key(psmin_wh, key, 1 );
        
        sprintf(buffer,std::string("Test string number %ld .......................................... %ld").c_str(), 1999999L-(((1999999L-ii))*3)%(2000000L), (((1999999L-ii))*3)%(2000000L));
        
        if (!obuffer || strcmp(buffer,obuffer)) {
            
            std::cout << "Failed to get string by key 1 " << (((1999999L-ii))*3)%(2000000L) << " from psmout_wh" << std::endl;
            
            std::cout << "Should be " << buffer << std::endl;
            
            if (obuffer) std::cout << "       is " << obuffer << std::endl;
            
        }
        
    }

    std::cout << "mapping write" << std::endl;

    
    if (PSM_write_file(psmin,std::string("PSM_test_file").c_str())) {
        
        std::cout << "Failed to write PSM_test_file " << ii << std::endl;
        
        return -1;
    }
    
    if (PSM_write_file(psmin_wh,std::string("PSM_test_file_wh").c_str())) {
    
        std::cout << "Failed to write PSM_test_file " << ii << std::endl;
        
        return -1;
    }

    std::cout << "mapping read" << std::endl;

    
    psmout = PSM_mmap_file(std::string("PSM_test_file").c_str());
    
    if (!psmout) {
        
        std::cout << "Failed to map PSM_test_file " << std::endl;
        
        return -1;
        
    }
    
    psmout_wh = PSM_mmap_file(std::string("PSM_test_file_wh").c_str());
    
    if (!psmout_wh) {
        
        std::cout << "Failed to map PSM_test_file_wh " << std::endl;
        
        return -1;
        
    }

    
    
    /* Post-mapping tests */
    
    std::cout << "Starting post-mapping tests" << std::endl;

    for (ii=0; ii < 2000000L; ii++) {
        
        if (ii%20000 == 0) std::cout << "Pass: " << ii << std::endl;
        
        sprintf(buffer,std::string("Test string number %ld .......................................... %ld").c_str(), ii, 1999999L-ii);
        
        sprintf(key,std::string("%ld").c_str(), (((1999999L-ii))*3)%(2000000L));

        obuffer = PSM_getstr(psmout,ii);
        
        if (!obuffer || strcmp(buffer,obuffer)) {
            
            std::cout << "Failed to get string " << ii << " from psmout" << std::endl;
            
            std::cout << "Should be " << buffer << std::endl;
            
            if (obuffer) std::cout << "       is " << obuffer << std::endl;
            
        }
        
        if (obuffer) free(obuffer);
        
        obuffer = PSM_getstr(psmout_wh,ii);
        
        if (strcmp(buffer,obuffer)) {
            
            std::cout << "Failed to get string " << ii << " from psmout_wh" << std::endl;
            
            std::cout << "Should be " << buffer << std::endl;
            
            std::cout << "       is " << obuffer << std::endl;
            
        }
        
        if (obuffer) free(obuffer);
        
        obuffer = PSM_getstr_by_key(psmout_wh, key, 0 );
        
        sprintf(buffer,std::string("Test string number %ld .......................................... %ld").c_str(), (((1999999L-ii))*3)%(2000000L), 1999999L-(((1999999L-ii))*3)%(2000000L));
        
        if (!obuffer || strcmp(buffer,obuffer)) {
            
            std::cout << "Failed to get string by key " << (((1999999L-ii))*3)%(2000000L) << " from psmout_wh" << std::endl;
            
            std::cout << "Should be " << buffer << std::endl;
            
            if (obuffer) std::cout << "       is " << obuffer << std::endl;
            
    }
    
        if (obuffer) free(obuffer);
        
    }
    
    return 0;
}
