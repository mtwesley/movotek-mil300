#include "cookshop.h"

#if 0

            'id': self.id,
            'type': self.type,
            'number': self.number,
            'status': self.status_cache,
            'instructions': self.instructions,
            'directions': self.delivery_directions(),
            'total': self.total(),
            'paid': self.paid(),
            'due': self.due(),
            'cash_due': self.due() + self.all_payment_amount(type='C', status='A'),
            'owed': self.owed(),
            'extra': self.extra(),
            'fee': self.fee(),
            'discount': self.discount(),
            'currency': self.currency,
            'location_id': self.location.id,
            'location': None if 'location' not in list(expand_ids) else self.location.to_api(fields=['id', 'code', 'name']),
            'user_id': self.user.id,
            'user': None if 'user' not in list(expand_ids) else self.user.to_api(fields=['id', 'name', 'email', 'phone']),
            'items': [{
                'id': i.item.id,
                'name': i.item.name,
                'code': i.item.code,
                'restaurant_id': i.item.restaurant.id,
                'restaurant': None if 'items.restaurant' not in list(expand_ids) else i.item.restaurant.to_api(fields=['id', 'code', 'name']),
                'quantity': i.quantity,
                'price': i.price,
                'margin': i.margin,
                'currency': i.currency,
            } for i in self.items],
            'timestamp': epoch(self.modified_timestamp)


#endif


order_t parse_order() {
    // bencode_t *ben
    order_t order;

    char *buf = "d8:cash_due4:2.008:currency3:USD10:directions7:ghghghf8:discounti0e3:due4:0.005:extrai0e3:fee1:02:idi25494e12:instructions0:5:itemsld4:code0:8:currency3:USD2:idi858e6:margin4:0.004:name19:Small Water (500ml)5:price4:2.008:quantityi1e10:restaurantd4:code3:DRK2:idi16e4:name6:Drinkse13:restaurant_idi16eee8:locationd4:code3:MPT2:idi1e4:name11:Mamba Pointe11:location_idi1e6:numberi125494e4:owed4:0.004:paid4:2.006:status1:P9:timestampi1503243805e5:total4:2.004:type1:D4:userd5:email27:mlentoo.wesley@cookshop.biz2:idi4e4:name15:Mlen-Too Wesley5:phone10:0776034108e7:user_idi4ee";
    int len = strlen(buf);

    bencode_t ben;
    bencode_init(&ben, buf, len);

    if (!bencode_is_dict(&ben)) return;

    while (bencode_dict_has_next(&ben)) {
        int klen, len;
        const char *key;
        bencode_t benk;

        bencode_dict_get_next(&ben, &benk, &key, &klen);

        if (!strncmp(key, "id", klen)) {
            bencode_int_value(&benk, order->id);
        }
        else if (!strncmp(key, "type", klen)) {
            bencode_string_value(&benk, order->type, &len);
        }
        else if (!strncmp(key, "number", klen)) {
            bencode_int_value(&benk, order->number);
        }
        else if (!strncmp(key, "status", klen)) {
            bencode_string_value(&benk, order->status, &len);
        }
        else if (!strncmp(key, "instructions", klen)) {
            bencode_string_value(&benk, order->instructions, &len);
        }
        else if (!strncmp(key, "directions", klen)) {
            bencode_string_value(&benk, order->directions, &len);
        }
        else if (!strncmp(key, "total", klen)) {
            bencode_string_value(&benk, order->total, &len);
        }
        else if (!strncmp(key, "paid", klen)) {
            bencode_string_value(&benk, order->paid, &len);
        }
        else if (!strncmp(key, "due", klen)) {
            bencode_string_value(&benk, order->due, &len);
        }
        else if (!strncmp(key, "cash_due", klen)) {
            bencode_string_value(&benk, order->cash_due, &len);
        }
        else if (!strncmp(key, "owed", klen)) {
            bencode_string_value(&benk, order->owed, &len);
        }
        else if (!strncmp(key, "extra", klen)) {
            bencode_string_value(&benk, order->extra, &len);
        }
        else if (!strncmp(key, "fee", klen)) {
            bencode_string_value(&benk, order->fee, &len);
        }
        else if (!strncmp(key, "discount", klen)) {
            bencode_string_value(&benk, order->discount, &len);
        }
        else if (!strncmp(key, "currency", klen)) {
            bencode_string_value(&benk, order->currency, &len);
        }
        else if (!strncmp(key, "location", klen)) {
            if (bencode_is_dict(&benk)) {
                while (bencode_dict_has_next(&benk)) {
                    int klen2, len2;
                    const char *key2;
                    bencode_t benk2;

                    bencode_dict_get_next(&benk, &benk2, &key2, &klen2);

                    if (!strncmp(key2, "name", klen2)) {
                        bencode_string_value(&benk2, order->location, &len2);
                    }
                }
            }
        }
        else if (!strncmp(key, "timestamp", klen)) {
            bencode_int_value(&benk, order->timestamp);
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
                            int klen2, len2;
                            const char *key2;
                            bencode_t benik;
                            *order_item_t order_item = (order->items + order->items_length - 1);
                            
                            bencode_dict_get_next(&beni, &benik, &key2, &klen2);                            

                            if (!strncmp(key2, "id", klen2)) {
                                bencode_int_value(&benik, order_item->id);
                            }
                            else if (!strncmp(key2, "name", klen2)) {
                                bencode_string_value(&benik, order_item->name, &len2);
                            }
                            else if (!strncmp(key2, "code", klen2)) {
                                bencode_string_value(&benik, order_item->code);
                            }
                            else if (!strncmp(key2, "restaurant", klen2)) {
                                if (bencode_is_dict(&benik)) {
                                    while (bencode_dict_has_next(&benik)) {
                                        int klen3, len3;
                                        const char *key3;
                                        bencode_t benk3;
                    
                                        bencode_dict_get_next(&benik, &benk3, &key3, &klen3);
                    
                                        if (!strncmp(key3, "name", klen3)) {
                                            bencode_string_value(&benk3, order_item->restaurant, &len3);
                                        }
                                    }
                                }
                            }
                            else if (!strncmp(key2, "quantity", klen2)) {
                                bencode_int_value(&benik, order_item->quantity);
                            }
                            else if (!strncmp(key2, "price", klen2)) {
                                bencode_string_value(&benik, order_item->price, &len2);
                            }
                            else if (!strncmp(key2, "margin", klen2)) {
                                bencode_string_value(&benik, order_item->margin, &len2);
                            }
                            else if (!strncmp(key2, "currency", klen2)) {
                                bencode_string_value(&benik, order_item->currency, &len2);
                            }
                        }
                    }
                }    
            }
        }
    }
}