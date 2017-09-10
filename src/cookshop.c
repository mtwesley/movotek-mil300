#include "cookshop.h"
#include "bencode.h"

void order_parse(order_t *order) {    
    bencode_t ben;
    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return;

    while (bencode_dict_has_next(&ben)) {
        long int int_val;
        const char *str_val;
        int klen, len;
        const char *key;
        bencode_t benk;

        bencode_dict_get_next(&ben, &benk, &key, &klen);

        if (!strncmp(key, "id", klen)) {
            bencode_int_value(&benk, &int_val);
            order->id = int_val;
        }
        else if (!strncmp(key, "type", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            order->type = str_val[0];
        }
        else if (!strncmp(key, "number", klen)) {
            bencode_int_value(&benk, &int_val);
            order->number = int_val;            
        }
        else if (!strncmp(key, "status", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            order->status = str_val[0];
        }
        else if (!strncmp(key, "timestamp", klen)) {
            bencode_int_value(&benk, &int_val);
            order->timestamp = int_val;            
        }
        else if (!strncmp(key, "items", klen)) {
            if (bencode_is_list(&benk)) {
                order->items_length = 0;
                while (bencode_list_has_next(&benk)) {                    
                    bencode_t beni;
                    bencode_list_get_next(&benk, &beni);
                    if (bencode_is_dict(&beni)) order->items_length++;
                }
            }
        }
    }
}

int order_get_instructions(order_t *order, char *instructions) {
    bencode_t ben;
    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return;

    while (bencode_dict_has_next(&ben)) {
        long int int_val;
        const char *str_val;
        int klen, len;
        const char *key;
        bencode_t benk;

        bencode_dict_get_next(&ben, &benk, &key, &klen);

        if (!strncmp(key, "instructions", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            strncpy(instructions, str_val, len);
        }
    }
    return 0;
}

int order_get_directions(order_t *order, char *directions) {
    bencode_t ben;
    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return;

    while (bencode_dict_has_next(&ben)) {
        long int int_val;
        const char *str_val;
        int klen, len;
        const char *key;
        bencode_t benk;

        bencode_dict_get_next(&ben, &benk, &key, &klen);

        if (!strncmp(key, "directions", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            strncpy(directions, str_val, len);
        }
    }
    return 0;
}

int order_get_total(order_t *order, char *total) {
    bencode_t ben;
    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return;

    while (bencode_dict_has_next(&ben)) {
        long int int_val;
        const char *str_val;
        int klen, len;
        const char *key;
        bencode_t benk;

        bencode_dict_get_next(&ben, &benk, &key, &klen);

        if (!strncmp(key, "total", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            strncpy(total, str_val, len);
        }
    }
    return 0;
}

int order_get_paid(order_t *order, char *paid) {
    bencode_t ben;
    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return;

    while (bencode_dict_has_next(&ben)) {
        long int int_val;
        const char *str_val;
        int klen, len;
        const char *key;
        bencode_t benk;

        bencode_dict_get_next(&ben, &benk, &key, &klen);

        if (!strncmp(key, "paid", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            strncpy(paid, str_val, len);
        }
    }
    return 0;
}
        
int order_get_due(order_t *order, char *due) {
    bencode_t ben;
    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return;

    while (bencode_dict_has_next(&ben)) {
        long int int_val;
        const char *str_val;
        int klen, len;
        const char *key;
        bencode_t benk;

        bencode_dict_get_next(&ben, &benk, &key, &klen);

        if (!strncmp(key, "due", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            strncpy(due, str_val, len);
        }
    }
    return 0;
}
        
int order_get_cash_due(order_t *order, char *cash_due) {
    bencode_t ben;
    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return;

    while (bencode_dict_has_next(&ben)) {
        long int int_val;
        const char *str_val;
        int klen, len;
        const char *key;
        bencode_t benk;

        bencode_dict_get_next(&ben, &benk, &key, &klen);

        if (!strncmp(key, "cash_due", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            strncpy(cash_due, str_val, len);
        }
    }
    return 0;
}
        
int order_get_owed(order_t *order, char *owed) {
    bencode_t ben;
    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return;

    while (bencode_dict_has_next(&ben)) {
        long int int_val;
        const char *str_val;
        int klen, len;
        const char *key;
        bencode_t benk;

        bencode_dict_get_next(&ben, &benk, &key, &klen);

        if (!strncmp(key, "owed", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            strncpy(owed, str_val, len);
        }
    }
    return 0;
}
        
int order_get_extra(order_t *order, char *extra) {
    bencode_t ben;
    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return;

    while (bencode_dict_has_next(&ben)) {
        long int int_val;
        const char *str_val;
        int klen, len;
        const char *key;
        bencode_t benk;

        bencode_dict_get_next(&ben, &benk, &key, &klen);

        if (!strncmp(key, "extra", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            strncpy(extra, str_val, len);
        }
    }
    return 0;
}
        
int order_get_fee(order_t *order, char *fee) {
    bencode_t ben;
    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return;

    while (bencode_dict_has_next(&ben)) {
        long int int_val;
        const char *str_val;
        int klen, len;
        const char *key;
        bencode_t benk;

        bencode_dict_get_next(&ben, &benk, &key, &klen);

        if (!strncmp(key, "fee", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            strncpy(fee, str_val, len);
        }
    }
    return 0;
}
        
int order_get_discount(order_t *order, char *discount) {
    bencode_t ben;
    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return;

    while (bencode_dict_has_next(&ben)) {
        long int int_val;
        const char *str_val;
        int klen, len;
        const char *key;
        bencode_t benk;

        bencode_dict_get_next(&ben, &benk, &key, &klen);

        if (!strncmp(key, "discount", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            strncpy(discount, str_val, len);
        }
    }
    return 0;
}
        
int order_get_currency(order_t *order, char *currency) {
    bencode_t ben;
    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return;

    while (bencode_dict_has_next(&ben)) {
        long int int_val;
        const char *str_val;
        int klen, len;
        const char *key;
        bencode_t benk;

        bencode_dict_get_next(&ben, &benk, &key, &klen);

        if (!strncmp(key, "currency", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            strncpy(currency, str_val, len);
        }
    }
    return 0;
}
        
int order_get_location(order_t *order, char *location) {
    bencode_t ben;
    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return;

    while (bencode_dict_has_next(&ben)) {
        long int int_val;
        const char *str_val;
        int klen, len;
        const char *key;
        bencode_t benk;

        bencode_dict_get_next(&ben, &benk, &key, &klen);

        if (!strncmp(key, "location", klen)) {
            if (bencode_is_dict(&benk)) {
                while (bencode_dict_has_next(&benk)) {
                    long int int_val2;
                    const char *str_val2;
                    int klen2, len2;
                    const char *key2;
                    bencode_t benk2;

                    bencode_dict_get_next(&benk, &benk2, &key2, &klen2);

                    if (!strncmp(key2, "name", klen2)) {
                        bencode_string_value(&benk2, &str_val2, &len2);
                        strncpy(location, str_val2, len2);
                    }
                }
            }
        }
    }
    return 0;
}

int order_get_item(order_t *order, int item, unsigned int *id, char *name, char *code) {
    bencode_t ben;
    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return;

    while (bencode_dict_has_next(&ben)) {
        long int int_val;
        const char *str_val;
        int klen, len;
        const char *key;
        bencode_t benk;

        bencode_dict_get_next(&ben, &benk, &key, &klen);

        if (!strncmp(key, "items", klen)) {
            if (bencode_is_list(&benk)) {
                int count = 0;
                while (bencode_list_has_next(&benk)) {                    
                    bencode_t beni;

                    bencode_list_get_next(&benk, &beni);
                    if (bencode_is_dict(&beni)) {
                        if (item == count++) {
                            while (bencode_dict_has_next(&beni)) {
                                long int int_val3;
                                const char *str_val3;
                                int klen3, len3;
                                const char *key3;
                                bencode_t benik;
                                
                                bencode_dict_get_next(&beni, &benik, &key3, &klen3);                            

                                if (!strncmp(key3, "id", klen3)) {
                                    bencode_int_value(&benik, &int_val3);
                                    *id = int_val3;
                                }
                                else if (!strncmp(key3, "name", klen3)) {
                                    bencode_string_value(&benik, &str_val3, &len3);
                                    strncpy(name, str_val3, len3);
                                }
                                else if (!strncmp(key3, "code", klen3)) {
                                    bencode_string_value(&benik, &str_val3, &len3);
                                    strncpy(code, str_val3, len3);
                                }
                            }
                        }
                    }
                }    
            }
        }
    }
    return 0;
}

int order_get_item_restaurant(order_t *order, int item, unsigned int *id, char *code, char *name) {
    bencode_t ben;
    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return;

    while (bencode_dict_has_next(&ben)) {
        long int int_val;
        const char *str_val;
        int klen, len;
        const char *key;
        bencode_t benk;

        bencode_dict_get_next(&ben, &benk, &key, &klen);

        if (!strncmp(key, "items", klen)) {
            if (bencode_is_list(&benk)) {
                int count = 0;
                while (bencode_list_has_next(&benk)) {                    
                    bencode_t beni;

                    bencode_list_get_next(&benk, &beni);
                    if (bencode_is_dict(&beni)) {
                        if (item == count++) {
                            while (bencode_dict_has_next(&beni)) {
                                long int int_val3;
                                const char *str_val3;
                                int klen3, len3;
                                const char *key3;
                                bencode_t benik;
                                
                                bencode_dict_get_next(&beni, &benik, &key3, &klen3);                            

                                if (!strncmp(key3, "restaurant", klen3)) {
                                    if (bencode_is_dict(&benik)) {
                                        while (bencode_dict_has_next(&benik)) {
                                            long int int_val4;
                                            const char *str_val4;
                                            int klen4, len4;
                                            const char *key4;
                                            bencode_t benk3;
                        
                                            bencode_dict_get_next(&benik, &benk3, &key4, &klen4);
                        
                                            if (!strncmp(key4, "id", klen4)) {
                                                bencode_int_value(&benk3, &int_val4);
                                                *id = int_val4;
                                            }
                                            else if (!strncmp(key4, "code", klen4)) {
                                                bencode_string_value(&benk3, &str_val4, &len3);
                                                strncpy(code, str_val4, len3);
                                            }
                                            else if (!strncmp(key4, "name", klen4)) {
                                                bencode_string_value(&benk3, &str_val4, &len3);
                                                strncpy(name, str_val4, len3);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }    
            }
        }
    }
    return 0;
}

int order_get_item_price(order_t *order, int item, char *price) {
    bencode_t ben;
    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return;

    while (bencode_dict_has_next(&ben)) {
        long int int_val;
        const char *str_val;
        int klen, len;
        const char *key;
        bencode_t benk;

        bencode_dict_get_next(&ben, &benk, &key, &klen);
        
        if (!strncmp(key, "items", klen)) {
            if (bencode_is_list(&benk)) {
                int count = 0;
                while (bencode_list_has_next(&benk)) {                    
                    bencode_t beni;

                    bencode_list_get_next(&benk, &beni);
                    if (bencode_is_dict(&beni)) {
                        if (item == count++) {
                            while (bencode_dict_has_next(&beni)) {
                                long int int_val3;
                                const char *str_val3;
                                int klen3, len3;
                                const char *key3;
                                bencode_t benik;
                                
                                bencode_dict_get_next(&beni, &benik, &key3, &klen3);                            

                                if (!strncmp(key3, "price", klen3)) {
                                    bencode_string_value(&benik, &str_val3, &len3);
                                    strncpy(price, str_val3, len3);
                                }
                            }
                        }
                    }
                }    
            }
        }
    }
    return 0;
}

int order_get_item_margin(order_t *order, int item, char *margin) {
    bencode_t ben;
    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return;

    while (bencode_dict_has_next(&ben)) {
        long int int_val;
        const char *str_val;
        int klen, len;
        const char *key;
        bencode_t benk;

        bencode_dict_get_next(&ben, &benk, &key, &klen);
        
        if (!strncmp(key, "items", klen)) {
            if (bencode_is_list(&benk)) {
                int count = 0;
                while (bencode_list_has_next(&benk)) {                    
                    bencode_t beni;

                    bencode_list_get_next(&benk, &beni);
                    if (bencode_is_dict(&beni)) {
                        if (item == count++) {
                            while (bencode_dict_has_next(&beni)) {
                                long int int_val3;
                                const char *str_val3;
                                int klen3, len3;
                                const char *key3;
                                bencode_t benik;
                                
                                bencode_dict_get_next(&beni, &benik, &key3, &klen3);                            

                                if (!strncmp(key3, "margin", klen3)) {
                                    bencode_string_value(&benik, &str_val3, &len3);
                                    strncpy(margin, str_val3, len3);
                                }
                            }
                        }
                    }
                }    
            }
        }
    }
    return 0;
}

int order_get_item_quantity(order_t *order, int item, char *quantity) {
    bencode_t ben;
    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return;

    while (bencode_dict_has_next(&ben)) {
        long int int_val;
        const char *str_val;
        int klen, len;
        const char *key;
        bencode_t benk;

        bencode_dict_get_next(&ben, &benk, &key, &klen);
        
        if (!strncmp(key, "items", klen)) {
            if (bencode_is_list(&benk)) {
                int count = 0;
                while (bencode_list_has_next(&benk)) {                    
                    bencode_t beni;

                    bencode_list_get_next(&benk, &beni);
                    if (bencode_is_dict(&beni)) {
                        if (item == count++) {
                            while (bencode_dict_has_next(&beni)) {
                                long int int_val3;
                                const char *str_val3;
                                int klen3, len3;
                                const char *key3;
                                bencode_t benik;
                                
                                bencode_dict_get_next(&beni, &benik, &key3, &klen3);                            

                                if (!strncmp(key3, "quantity", klen3)) {
                                    bencode_string_value(&benik, &str_val3, &len3);
                                    strncpy(quantity, str_val3, len3);
                                }
                            }
                        }
                    }
                }    
            }
        }
    }
    return 0;
}

int order_get_item_currency(order_t *order, int item, char *currency) {
    bencode_t ben;
    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return;

    while (bencode_dict_has_next(&ben)) {
        long int int_val;
        const char *str_val;
        int klen, len;
        const char *key;
        bencode_t benk;

        bencode_dict_get_next(&ben, &benk, &key, &klen);
        
        if (!strncmp(key, "items", klen)) {
            if (bencode_is_list(&benk)) {
                int count = 0;
                while (bencode_list_has_next(&benk)) {                    
                    bencode_t beni;

                    bencode_list_get_next(&benk, &beni);
                    if (bencode_is_dict(&beni)) {
                        if (item == count++) {
                            while (bencode_dict_has_next(&beni)) {
                                long int int_val3;
                                const char *str_val3;
                                int klen3, len3;
                                const char *key3;
                                bencode_t benik;
                                
                                bencode_dict_get_next(&beni, &benik, &key3, &klen3);                            

                                if (!strncmp(key3, "currency", klen3)) {
                                    bencode_string_value(&benik, &str_val3, &len3);
                                    strncpy(currency, str_val3, len3);
                                }
                            }
                        }
                    }
                }    
            }
        }
    }
    return 0;
}
