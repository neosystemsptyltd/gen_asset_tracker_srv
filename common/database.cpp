#include "database.h"
#include "custlog.h"
#include <string>
#include <memory>

namespace Neo {
    
namespace neotrac {
    
namespace Database {

    using namespace std;
    using namespace Neo::neotrac::Log;
    
NeotracDataBase::NeotracDataBase(string& username, string& password)
{
    log_trace("Instantiate the database class: U:" + username + " p:" + password);
    
    _user   = username;
    _passwd = password;
}

NeotracDataBase::~NeotracDataBase()
{
    log_trace("Destroy the database class");    
}

std::string NeotracDataBase::getDescr()
{
    return "Neo Systems database. Postgresql: User=" + _user;
}

bool NeotracDataBase::Connect()
{
    return false;
}

bool NeotracDataBase::Disconnect()
{
    return false;    
}

bool NeotracDataBase::FindDeviceByPrimaryIMEI(std::string& IMEI, DATABASE_DEVICE_ST* DeviceStP)
{
    log_trace("IMEI: " + IMEI);
    return DeviceStP == NULL;    
}

bool NeotracDataBase::InsertDevice(DATABASE_DEVICE_ST* DeviceStP)
{
    
    return DeviceStP == NULL;
}

bool NeotracDataBase::UpdateDeviceByPrimaryIMEI(DATABASE_DEVICE_ST* DeviceStP)
{
    return (DeviceStP == NULL);    
}
    
}    // end namespaces
}    
}
