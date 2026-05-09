#ifndef WEBSERVERMANAGER_H
#define WEBSERVERMANAGER_H
 
#include "storage/MemoriaSistema.h"
 
class WebServerManager {
public:
    void begin(MemoriaSistema& mem);
    void handle();
};
 
#endif
 
