#include "cookshop.h"
#include "bencode.h"

order_t parse_order() {
    // bencode_t *ben
    order_t *order;

    char *buf = "d8:cash_due4:2.008:currency3:USD10:directions7:ghghghf8:discounti0e3:due4:0.005:extrai0e3:fee1:02:idi25494e12:instructions0:5:itemsld4:code0:8:currency3:USD2:idi858e6:margin4:0.004:name19:Small Water (500ml)5:price4:2.008:quantityi1e10:restaurantd4:code3:DRK2:idi16e4:name6:Drinkse13:restaurant_idi16eee8:locationd4:code3:MPT2:idi1e4:name11:Mamba Pointe11:location_idi1e6:numberi125494e4:owed4:0.004:paid4:2.006:status1:P9:timestampi1503243805e5:total4:2.004:type1:D4:userd5:email27:mlentoo.wesley@cookshop.biz2:idi4e4:name15:Mlen-Too Wesley5:phone10:0776034108e7:user_idi4ee";
    int len = strlen(buf);

    bencode_t ben;
    bencode_init(&ben, buf, len);

    if (!bencode_is_dict(&ben)) return;

    order = (order_t *) malloc(sizeof(order_t));

    while (bencode_dict_has_next(&ben)) {
        long int int_val;
        const char str_val;
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
            &order->type = str_val;
        }
        else if (!strncmp(key, "number", klen)) {
            bencode_int_value(&benk, &int_val);
            order->number = int_val;            
        }
        else if (!strncmp(key, "status", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            order->status = str_val;
        }
        else if (!strncmp(key, "instructions", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            order->instructions = str_val;
        }
        else if (!strncmp(key, "directions", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            order->directions = str_val;
        }
        else if (!strncmp(key, "total", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            order->total = str_val;
        }
        else if (!strncmp(key, "paid", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            order->paid = str_val;
        }
        else if (!strncmp(key, "due", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            order->due = str_val;
        }
        else if (!strncmp(key, "cash_due", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            order->cash_due = str_val;
        }
        else if (!strncmp(key, "owed", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            order->owed = str_val;
        }
        else if (!strncmp(key, "extra", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            order->extra = str_val;
        }
        else if (!strncmp(key, "fee", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            order->fee = str_val;
        }
        else if (!strncmp(key, "discount", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            order->discount = str_val;
        }
        else if (!strncmp(key, "currency", klen)) {
            bencode_string_value(&benk, &str_val, &len);
            order->currency = str_val;
        }
        else if (!strncmp(key, "location", klen)) {
            if (bencode_is_dict(&benk)) {
                while (bencode_dict_has_next(&benk)) {
                    long int int_val2;
                    const char str_val2;
                    int klen2, len2;
                    const char *key2;
                    bencode_t benk2;

                    bencode_dict_get_next(&benk, &benk2, &key2, &klen2);

                    if (!strncmp(key2, "name", klen2)) {
                        bencode_string_value(&benk2, &str_val2, &len2);
                        order->location = str_val2;
                    }
                }
            }
        }
        else if (!strncmp(key, "timestamp", klen)) {
            bencode_int_value(&benk, &int_val);
            order->timestamp = int_val;            
        }
        else if (!strncmp(key, "items", klen)) {
            if (bencode_is_list(&benk)) {
                order->items_length = 0;
                order->items = (order_item_t *) malloc(sizeof(order_item_t));
                while (bencode_list_has_next(&benk)) {                    
                    bencode_t beni;

                    bencode_list_get_next(&benk, &beni);
                    if (bencode_is_dict(&beni)) {
                        order->items_length++;
                        order->items = realloc(order->items, sizeof(order_item_t) * order->items_length);

                        while (bencode_dict_has_next(&beni)) {
                            long int int_val3;
                            const char str_val3;
                            int klen3, len3;
                            const char *key3;
                            bencode_t benik;
                            order_item_t *order_item = (order->items + order->items_length - 1);
                            
                            bencode_dict_get_next(&beni, &benik, &key3, &klen3);                            

                            if (!strncmp(key3, "id", klen3)) {
                                bencode_int_value(&benik, &int_val3);
                                order_item->id = int_val3;
                            }
                            else if (!strncmp(key3, "name", klen3)) {
                                bencode_string_value(&benik, &str_val3, &len3);
                                order_item->name = str_val3;
                            }
                            else if (!strncmp(key3, "code", klen3)) {
                                bencode_string_value(&benik, &str_val3, &len3);
                                order_item->code = str_val3;
                            }
                            else if (!strncmp(key3, "restaurant", klen3)) {
                                if (bencode_is_dict(&benik)) {
                                    while (bencode_dict_has_next(&benik)) {
                                        int klen3, len3;
                                        const char *key3;
                                        bencode_t benk3;
                    
                                        bencode_dict_get_next(&benik, &benk3, &key3, &klen3);
                    
                                        if (!strncmp(key3, "name", klen3)) {
                                            bencode_string_value(&benk3, &str_val3, &len3);
                                            order_item->restaurant = str_val3;
                                        }
                                    }
                                }
                            }
                            else if (!strncmp(key3, "quantity", klen3)) {
                                bencode_int_value(&benik, &int_val3);
                                order_item->quantity = int_val3;
                            }
                            else if (!strncmp(key3, "price", klen3)) {
                                bencode_string_value(&benik, &str_val3, &len3);
                                order_item->price = str_val3;
                            }
                            else if (!strncmp(key3, "margin", klen3)) {
                                bencode_string_value(&benik, &str_val3, &len3);
                                order_item->margin = str_val3;
                            }
                            else if (!strncmp(key3, "currency", klen3)) {
                                bencode_string_value(&benik, &str_val3, &len3);
                                order_item->currency = str_val3;
                            }
                        }
                    }
                }    
            }
        }
    }
}