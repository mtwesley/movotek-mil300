#ifndef COOKSHOP_H
#define	COOKSHOP_H

typedef struct order_item_t {
    unsigned int id;
    char name[100];
    char code[50];
    char restaurant[50];
    unsigned int quantity;
    char price[6];
    char margin[6];
    char currency[3];
} order_item_t;

typedef struct order_t {
    unsigned int id;
    char type;
    unsigned int number;
    char status;
    char instructions[500];
    char directions[500];
    char total[6];
    char paid[6];
    char due[6];
    char cash_due[6];
    char owed[6];
    char extra[6];
    char fee[6];
    char discount[6];
    char currency[3];
    char location[50];
    unsigned long int timestamp;
    order_item_t *items;
    int items_length;
} order_t;

#endif
