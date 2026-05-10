#ifndef WEBSERVERMANAGER_H
#define WEBSERVERMANAGER_H
 
#include "MemoriaSistema.h"
 
class WebServerManager {
public:
    void begin(MemoriaSistema& mem);
    void handle();
};
 
#endif
 
