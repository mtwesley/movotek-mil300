#include <time.h>
#include "sms_pdu.h"

#ifndef COOKSHOP_H
#define	COOKSHOP_H

#define ORDERS_NEW "NEW"
#define ORDERS_PENDING "PENDNG"
#define ORDERS_PICKED_UP "PICKUP"
#define ORDERS_DELIVERED "DELVRD"

typedef struct order_t {
    unsigned long int id;
    char type;
    unsigned long int number;
    char status;
    unsigned long int timestamp;
    unsigned char bencode[SMS_MESSAGE_LENGTH];
    int items_length;
} order_t;

#endif
