#ifndef DATABASE_H
#define DATABASE_H

#include "neoprotocol.h"
#include <string>
#include <memory>

namespace Neo {

namespace neotrac {
    
namespace Database {

using namespace Neo::neotrac::Protocol;
using namespace std;

    typedef struct {
        NEOPROTOCOL_ST_REGISTRATION RegInfo;
                
        char privatekey[80];
        char description[100];
    } DATABASE_DEVICE_ST;
    
    class NeotracDataBase 
    {
    public:        
        explicit NeotracDataBase(string& username, string& password);
        ~NeotracDataBase();
        
        std::string getDescr();
        bool Connect();        
        bool Disconnect();
        
        bool FindDeviceByPrimaryIMEI(string& IMEI, DATABASE_DEVICE_ST* DeviceStP);        
        bool InsertDevice(DATABASE_DEVICE_ST* DeviceStP);
        bool UpdateDeviceByPrimaryIMEI(DATABASE_DEVICE_ST* DeviceStP);
        
    private:
        string _user;
        string _passwd;
    };
    
}}}


#endif
