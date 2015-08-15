#ifndef NEOPROTOCOL_H
#define NEOPROTOCOL_H

#include "cassert.h"

#ifdef __cplusplus
namespace Neo {
    
namespace neotrac {
        
namespace Protocol {
    
using namespace std;    
#endif

typedef enum {
    NEOPROTOCOL_MESSAGE_TYPE_E_REGISTRATION,
        
    NEOPROTOCOL_MESSAGE_TYPE_E_LAST        
} NEOPROTOCOL_MESSAGE_TYPE_E;

typedef struct __attribute__((packed)) {
    char imei1[18];
    char imei2[18];
    char imei3[18];
    char hwserial[20];
    char rfmac[30];
    char wifimac[30];
    
    uint16_t CrcU16;
} NEOPROTOCOL_ST_REGISTRATION;
CASSERT(sizeof(NEOPROTOCOL_ST_REGISTRATION) == 136, sizeof_NEOPROTOCOL_ST_REGISTRATION_not_correct)

typedef struct __attribute__((packed)) {
    
	int32_t  lattitude;
    int32_t  longitude;
    uint32_t time;
    
	uint16_t eventcode;
	uint16_t direction;
    uint16_t inputs;
    uint16_t outputs;
    
	int16_t analog0;
	int16_t analog1;
	int16_t analog2;
	int16_t analog3;
    
	uint8_t  speed;
    
	uint8_t gpsstatus;
	int8_t  gpsrssi;
	
	uint8_t gsmstatus;
	int8_t  gsmrssi;
	
	uint8_t rfstatus;
	int8_t  rfrssi;
	
	uint8_t btstatus;
	int8_t  btrssi;
	
	uint8_t wifistatus;
	int8_t  wifirssi;
	
	uint8_t satmodemstatus;
	int8_t  satmodemrssi;

	uint8_t pwrstatmask;
	uint8_t systemstatmask;
	uint8_t driverstatmask;
	uint8_t sysfailstatmask;
	uint8_t analogstatmask;
		
	uint8_t bckbatstatus;
	
    uint16_t CrcU16;
} NEOPROTOCOL_ST_EVENT;
CASSERT(sizeof(NEOPROTOCOL_ST_EVENT) == 49, sizeof_NEOPROTOCOL_ST_EVENT_not_correct)

typedef union {
    NEOPROTOCOL_ST_REGISTRATION Registration;
    NEOPROTOCOL_ST_EVENT        Event;
} NEPROTOCOL_BODY_UN;

/*
 * packet that can be sent over SMS
 */
typedef struct __attribute__((packed)) {
    NEOPROTOCOL_MESSAGE_TYPE_E MessageTypeE;
    uint8_t                    ProtocolVersion;
    uint8_t                    Encryptionkey[6];
    uint64_t                   PrimaryImei;
    NEPROTOCOL_BODY_UN         Body;
    
} NEOPROTOCOL_ST_SHORT_PACKET;
CASSERT(sizeof(NEOPROTOCOL_ST_SHORT_PACKET) < 158, sizeof_NEOPROTOCOL_ST_SHORT_PACKET_is_too_big)
    
#ifdef __cplusplus
}}} // end of namespace
#endif

#endif
