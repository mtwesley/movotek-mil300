#include "sms_pdu.h"

#ifndef COOKSHOP_H
#define	COOKSHOP_H

typedef struct order_t {
    unsigned long int id;
    char type;
    unsigned long int number;
    char status;
    unsigned int timestamp[8];
    char bencode[SMS_MESSAGE_LENGTH];
    int items_length;
} order_t;

// int sms_get_msg(char *msg, int *msg_len, int max_len)
// void order_parse(order_t *order);
// int order_get_instructions(order_t *order, char *instructions);
// int order_get_directions(order_t *order, char *directions);
// int order_get_total(order_t *order, char *total);
// int order_get_paid(order_t *order, char *paid);
// int order_get_due(order_t *order, char *due);
// int order_get_cash_due(order_t *order, char *cash_due);
// int order_get_owed(order_t *order, char *owed);
// int order_get_extra(order_t *order, char *extra);
// int order_get_fee(order_t *order, char *fee);
// int order_get_discount(order_t *order, char *discount);
// int order_get_currency(order_t *order, char *currency);
// int order_get_location(order_t *order, char *location);
// int order_get_item(order_t *order, int item, unsigned int *id, char *name, char *code);
// int order_get_item_restaurant(order_t *order, int item, unsigned int *id, char *code, char *name);
// int order_get_item_price(order_t *order, int item, char *price);
// int order_get_item_margin(order_t *order, int item, char *margin);
// int order_get_item_quantity(order_t *order, int item, char *quantity);
// int order_get_item_currency(order_t *order, int item, char *currency);


#endif
