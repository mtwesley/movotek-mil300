#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "public.h"
#include "cookshop.h"
#include "bencode.h"
#include "sms_pdu.h"

int sms_get_msg(char *msg, int *msg_len, int max_len) {
    int i, j;
    sms_t sms;

    char msg_parts[SMS_MULTIPART_MAX][SMS_MULTIPART_SIZE];
    ushort msg_ids[SMS_MULTIPART_MAX];

    ushort len, pdu_len, msg_id;
    unsigned char *rsp, *tmp, *pdu;
    unsigned char cmd[20];
    unsigned char buf[8000];

    int has_message = FALSE;
    int multipart_ref = 0;

    for (i = 0; i < SMS_MULTIPART_MAX; i++) {
        j = i + 1;
        memset(msg_parts[j], 0, sizeof(msg_parts[j]));
        msg_ids[j] = 0;
    }

    memset(buf, 0, 8000);
    strcpy(cmd, "AT+CMGL=4\r");
    if ((Wls_ExecuteCmd(cmd, strlen(cmd), buf, 8000, &len, 3000) == WLS_OK) && strlen(buf)) {
        tmp = buf;
        while (rsp = strstr(tmp, "+CMGL: ")) {
            pdu = strstr(rsp, "\r\n") + 2;
            tmp = strstr(pdu, "\r\n");
            pdu_len = tmp - pdu + 1;

            if (!sms_decode_pdu(pdu, pdu_len, &sms)) {
                int is_old_message = FALSE;                        
                unsigned char datetime[8];
                unsigned int new_datetime[8];

                sscanf(rsp + 7, "%D", &msg_id);
                Lib_GetDateTime(datetime);

                for (i = 0; i < 8; i++) new_datetime[i] = bin_ts(datetime[i]);

                int diff = datetime_to_epoch(new_datetime) - datetime_to_epoch(sms.timestamp);
                if (diff > 3600) {
                    sprintf(cmd, "AT+CMGD=%d,0\r", msg_id);
                    Wls_ExecuteCmd(cmd, strlen(cmd), buf, 8000, &len, 1000);
                    multipart_ref = 0;
                    continue;
                }

                if (sms.message_type & SMS_MULTIPART) {
                    int ref = sms.message_reference;
                    int num = sms.message_number;
                    int parts = sms.message_parts;

                    if (multipart_ref == 0) multipart_ref = ref;
                    else if (multipart_ref != ref) continue;

                    strncpy(msg_parts[num], sms.message, sms.message_length);
                    msg_ids[num] = msg_id;

                    for (i = 0; i < SMS_MULTIPART_MAX; i++) {
                        j = i + 1;
                        if (msg_ids[j] == 0) break;
                        else if (j == parts) {
                            has_message = TRUE;
                            multipart_ref = 0;
                        } 
                    }
                }
                else if (multipart_ref == 0) {
                    strncpy(msg_parts[1], sms.message, sms.message_length);
                    msg_ids[1] = msg_id;
                    has_message = TRUE;
                }
            }
            if (has_message) break;
        }

        if (has_message) {
            for (i = 0; i < SMS_MULTIPART_MAX; i++) {
                j = i + 1;
                if (msg_ids[j] != 0) {
                    sprintf(cmd, "AT+CMGD=%d,0\r", msg_ids[j]);
                    Wls_ExecuteCmd(cmd, strlen(cmd), buf, 8000, &len, 1000);
                }
                if (strlen(msg_parts[j])) strncat(msg, msg_parts[j], max_len);
            }
        }

        *msg_len = strlen(msg);
    }
    return 0;
}

int order_parse(order_t *order) {
    int klen0;
    const char *key0;
    bencode_t ben, benk;

    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return 0;
    while (bencode_dict_has_next(&ben) && bencode_dict_get_next(&ben, &benk, &key0, &klen0)) {
        if (!strncmp(key0, "orders", klen0) && bencode_is_list(&benk) && bencode_list_has_next(&benk)) {                    
            bencode_list_get_next(&benk, &ben);

            if (!bencode_is_dict(&ben)) continue;
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
            return 1;
        }
    }
    return 0;
}

int order_get_int(order_t *order, char *int_key, int *intv) {
    int klen0;
    const char *key0;
    bencode_t ben, benk;

    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return 0;
    while (bencode_dict_has_next(&ben) && bencode_dict_get_next(&ben, &benk, &key0, &klen0)) {
        if (!strncmp(key0, "orders", klen0) && bencode_is_list(&benk) && bencode_list_has_next(&benk)) {                    
            bencode_list_get_next(&benk, &ben);

            if (!bencode_is_dict(&ben)) continue;
            while (bencode_dict_has_next(&ben)) {
                long int int_val;
                const char *str_val;
                int klen, len;
                const char *key;
                bencode_t benk;

                bencode_dict_get_next(&ben, &benk, &key, &klen);

                if (!strncmp(key, int_key, klen)) {
                    bencode_int_value(&benk, &int_val);
                    *intv = int_val;
                    return 1;
                }
            }
        }
    }
    return 0;
}

int order_get_string(order_t *order, char *str_key, char *strv) {
    int klen0;
    const char *key0;
    bencode_t ben, benk;

    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return 0;
    while (bencode_dict_has_next(&ben) && bencode_dict_get_next(&ben, &benk, &key0, &klen0)) {
        if (!strncmp(key0, "orders", klen0) && bencode_is_list(&benk) && bencode_list_has_next(&benk)) {                    
            bencode_list_get_next(&benk, &ben);

            if (!bencode_is_dict(&ben)) continue;
            while (bencode_dict_has_next(&ben)) {
                long int int_val;
                const char *str_val;
                int klen, len;
                const char *key;
                bencode_t benk;

                bencode_dict_get_next(&ben, &benk, &key, &klen);

                if (!strncmp(key, str_key, klen)) {
                    bencode_string_value(&benk, &str_val, &len);
                    strncpy(strv, str_val, len);
                    return 1;
                }
            }
        }
    }
    return 0;
}

int order_get_instructions(order_t *order, char *instructions) {
    return order_get_string(order, "instructions", instructions);
}

int order_get_directions(order_t *order, char *directions) {
    return order_get_string(order, "directions", directions);
}

int order_get_total(order_t *order, char *total) {
    return order_get_string(order, "total", total);
}

int order_get_paid(order_t *order, char *paid) {
    return order_get_string(order, "paid", paid);
}
        
int order_get_due(order_t *order, char *due) {
    return order_get_string(order, "due", due);
}
        
int order_get_cash_due(order_t *order, char *cash_due) {
    return order_get_string(order, "cash_due", cash_due);
}
        
int order_get_owed(order_t *order, char *owed) {
    return order_get_string(order, "owed", owed);
}
        
int order_get_extra(order_t *order, char *extra) {
    return order_get_string(order, "extra", extra);
}
        
int order_get_fee(order_t *order, char *fee) {
    return order_get_string(order, "fee", fee);
}
        
int order_get_discount(order_t *order, char *discount) {
    return order_get_string(order, "discount", discount);
}
        
int order_get_currency(order_t *order, char *currency) {
    return order_get_string(order, "currency", currency);
}
     
int order_get_location(order_t *order, char *location) {
    int klen0;
    const char *key0;
    bencode_t ben, benk;

    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return 0;
    while (bencode_dict_has_next(&ben) && bencode_dict_get_next(&ben, &benk, &key0, &klen0)) {
        if (!strncmp(key0, "orders", klen0) && bencode_is_list(&benk) && bencode_list_has_next(&benk)) {                    
            bencode_list_get_next(&benk, &ben);

            if (!bencode_is_dict(&ben)) continue;
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
                                return 1;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

int order_get_user(order_t *order, unsigned long int *id, char *name, char *phone, char *email) {
    int klen0;
    const char *key0;
    bencode_t ben, benk;

    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return 0;
    while (bencode_dict_has_next(&ben) && bencode_dict_get_next(&ben, &benk, &key0, &klen0)) {
        if (!strncmp(key0, "orders", klen0) && bencode_is_list(&benk) && bencode_list_has_next(&benk)) {                    
            bencode_list_get_next(&benk, &ben);

            if (!bencode_is_dict(&ben)) continue;
            while (bencode_dict_has_next(&ben)) {
                long int int_val;
                const char *str_val;
                int klen, len;
                const char *key;
                bencode_t benk;

                bencode_dict_get_next(&ben, &benk, &key, &klen);

                if (!strncmp(key, "user", klen)) {
                    if (bencode_is_dict(&benk)) {
                        while (bencode_dict_has_next(&benk)) {
                            long int int_val2;
                            const char *str_val2;
                            int klen2, len2;
                            const char *key2;
                            bencode_t benk2;

                            bencode_dict_get_next(&benk, &benk2, &key2, &klen2);

                            if (!strncmp(key2, "id", klen2)) {
                                bencode_int_value(&benk2, &int_val2);
                                *id = int_val2;
                            }
                            else if (!strncmp(key2, "name", klen2)) {
                                bencode_string_value(&benk2, &str_val2, &len2);
                                strncpy(name, str_val2, len2);
                            }
                            else if (!strncmp(key2, "phone", klen2)) {
                                bencode_string_value(&benk2, &str_val2, &len2);
                                strncpy(phone, str_val2, len2);
                            }
                            else if (!strncmp(key2, "email", klen2)) {
                                bencode_string_value(&benk2, &str_val2, &len2);
                                strncpy(email, str_val2, len2);
                            }
                        }
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

int order_get_item(order_t *order, int item, unsigned int *id, char *name, char *code) {
    int klen0;
    const char *key0;
    bencode_t ben, benk;

    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return 0;
    while (bencode_dict_has_next(&ben) && bencode_dict_get_next(&ben, &benk, &key0, &klen0)) {
        if (!strncmp(key0, "orders", klen0) && bencode_is_list(&benk) && bencode_list_has_next(&benk)) {                    
            bencode_list_get_next(&benk, &ben);

            if (!bencode_is_dict(&ben)) continue;
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
                                    return 1;
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

int order_get_item_string(order_t *order, int item, char *str_key, char *strv) {
    int klen0;
    const char *key0;
    bencode_t ben, benk;

    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return 0;
    while (bencode_dict_has_next(&ben) && bencode_dict_get_next(&ben, &benk, &key0, &klen0)) {
        if (!strncmp(key0, "orders", klen0) && bencode_is_list(&benk) && bencode_list_has_next(&benk)) {                    
            bencode_list_get_next(&benk, &ben);

            if (!bencode_is_dict(&ben)) continue;
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

                                        if (!strncmp(key3, str_key, klen3)) {
                                            bencode_string_value(&benik, &str_val3, &len3);
                                            strncpy(strv, str_val3, len3);
                                        }
                                    }
                                    return 1;
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
    int klen0;
    const char *key0;
    bencode_t ben, benk;

    bencode_init(&ben, order->bencode, strlen(order->bencode));

    if (!bencode_is_dict(&ben)) return 0;
    while (bencode_dict_has_next(&ben) && bencode_dict_get_next(&ben, &benk, &key0, &klen0)) {
        if (!strncmp(key0, "orders", klen0) && bencode_is_list(&benk) && bencode_list_has_next(&benk)) {                    
            bencode_list_get_next(&benk, &ben);

            if (!bencode_is_dict(&ben)) continue;
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
                                                return 1;
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
    return order_get_item_string(order, item, "price", price);
}

int order_get_item_margin(order_t *order, int item, char *margin) {
    return order_get_item_string(order, item, "margin", margin);
}

int order_get_item_quantity(order_t *order, int item, char *quantity) {
    return order_get_item_string(order, item, "quantity", quantity);
}

int order_get_item_currency(order_t *order, int item, char *currency) {
    return order_get_item_string(order, item, "currency", currency);
}
