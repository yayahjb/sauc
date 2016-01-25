#include <iostream>
#include <string>
#include "pststrmgr.h"
#include "stdio.h"

int main (int argc, char ** argv) {
    
    size_t ii;
    
    char buffer[80];
    
    char* obuffer;
    
    PSM_localpsm_handle psmin, psmout;
    
    psmin = PSM_localpsm_handle_create();
    
    for (ii=0; ii < 2000000L; ii++) {
        
        sprintf(buffer,std::string("Test string number %ld ..........................................").c_str(), ii);
        
        if (!PSM_addstr(psmin,buffer)) {
            
            std::cerr << "Failed to add string " << ii << std::endl;
            
            return -1;
        }
    }
    
    if (PSM_write_file(psmin,std::string("PSM_test_file").c_str())) {
        
        std::cerr << "Failed to write PSM_test_file " << ii << std::endl;
        
        return -1;
    }
    
    
    psmout = PSM_mmap_file(std::string("PSM_test_file").c_str());
    
    if (!psmout) {
        
        std::cerr << "Failed to map PSM_test_file " << std::endl;
        
        return -1;
        
    }
    
    for (ii=0; ii < 2000000L; ii++) {
        
        sprintf(buffer,std::string("Test string number %ld ..........................................").c_str(), ii);
        
        obuffer = PSM_getstr(psmout,ii);
        
        if (strcmp(buffer,obuffer)) {
            
            std::cerr << "Failed to get string " << ii << std::endl;
            
            std::cerr << "Should be " << buffer << std::endl;
            
            std::cerr << "       is " << obuffer << std::endl;
            
        }
        
        free(obuffer);
    }
    
    return 0;
}
