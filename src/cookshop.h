#ifndef COOKSHOP_H
#define	COOKSHOP_H

#if 0

typedef struct order_item_t {
    unsigned int id;
    char name[100];
    char code[50];
    char restaurant[50];
    unsigned int quantity;
    char price[6];
    char margin[6];
    char currency[3];
    char *bencode
} order_item_t;

#endif

typedef struct order_t {
    unsigned long int id;
    char type;
    unsigned long int number;
    char status;
    unsigned long int timestamp;
    char *bencode;
    int items_length;
} order_t;

#endif
