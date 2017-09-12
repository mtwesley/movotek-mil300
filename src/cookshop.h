#ifndef COOKSHOP_H
#define	COOKSHOP_H

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
