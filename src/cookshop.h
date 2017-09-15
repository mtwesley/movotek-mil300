#ifndef COOKSHOP_H
#define	COOKSHOP_H

char *test_bencode = "d6:ordersld8:cash_due4:2.008:currency3:USD10:directions7:ghghghf8:discounti0e3:due4:0.005:extrai0e3:fee1:02:idi25494e12:instructions0:5:itemsld4:code0:8:currency3:USD2:idi858e6:margin4:0.004:name19:Small Water (500ml)5:price4:2.008:quantityi1e10:restaurantd4:code3:DRK2:idi16e4:name6:Drinkse13:restaurant_idi16eee8:locationd4:code3:MPT2:idi1e4:name11:Mamba Pointe11:location_idi1e6:numberi125494e4:owed4:0.004:paid4:2.006:status1:P9:timestampi1503243805e5:total4:2.004:type1:D4:userd5:email27:mlentoo.wesley@cookshop.biz2:idi4e4:name15:Mlen-Too Wesley5:phone10:0776034108e7:user_idi4eeee";

typedef struct order_t {
    unsigned long int id;
    char type;
    unsigned long int number;
    char status;
    unsigned long int timestamp;
    char *bencode;
    int items_length;
} order_t;

int sms_get_msg(char *msg, int *msg_len, int max_len)
void order_parse(order_t *order);
int order_get_instructions(order_t *order, char *instructions);
int order_get_directions(order_t *order, char *directions);
int order_get_total(order_t *order, char *total);
int order_get_paid(order_t *order, char *paid);
int order_get_due(order_t *order, char *due);
int order_get_cash_due(order_t *order, char *cash_due);
int order_get_owed(order_t *order, char *owed);
int order_get_extra(order_t *order, char *extra);
int order_get_fee(order_t *order, char *fee);
int order_get_discount(order_t *order, char *discount);
int order_get_currency(order_t *order, char *currency);
int order_get_location(order_t *order, char *location);
int order_get_item(order_t *order, int item, unsigned int *id, char *name, char *code);
int order_get_item_restaurant(order_t *order, int item, unsigned int *id, char *code, char *name);
int order_get_item_price(order_t *order, int item, char *price);
int order_get_item_margin(order_t *order, int item, char *margin);
int order_get_item_quantity(order_t *order, int item, char *quantity);
int order_get_item_currency(order_t *order, int item, char *currency);


#endif
