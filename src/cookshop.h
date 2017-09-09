
typedef struct order_item_t {
    unsigned int id;
    char[100] name;
    char[50] code;
    char[50] restaurant;
    unsigned int quantity;
    char[6] price;
    char[6] margin;
    char[3] currency;
} order_item_t;

typedef struct order_t {
    unsigned int id;
    char type;
    unsigned int number;
    char status;
    char[500] instructions;
    char[500] directions;
    char[6] total;
    char[6] paid;
    char[6] due;
    char[6] cash_due;
    char[6] owed;
    char[6] extra;
    char[6] fee;
    char[6] discount;
    char[3] currency;
    char[50] location;
    unsigned long int timestamp;
    order_item_t *items;
    in items_length;
} order_t;

