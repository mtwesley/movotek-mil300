#ifndef SMS_PDU_H
#define	SMS_PDU_H

#define SMS_MESSAGE_BUFFER  180
#define SMS_MESSAGE_SIZE    161
#define SMS_MULTIPART_SIZE  154
#define SMS_SENDER_SIZE     25

#define SMS_SINGLE          0x01
#define SMS_MULTIPART       0x40

#define SMS_MULTIPART_MAX   32
#define SMS_MESSAGE_MINUTES 60
#define SMS_MESSAGE_LENGTH  4000
#define SMS_BUFFER_LENGTH   16000

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

/* 
* Please refer to:
*   http://www.dreamfabric.com/sms/type_of_address.html
*/    
typedef struct sms_t {
    int id;
    unsigned char message_type;
    unsigned char message[SMS_MESSAGE_BUFFER];
    size_t message_length;
    uint8_t message_reference;
    uint8_t message_parts;
    uint8_t message_number;
    uint8_t telnum_type;
    char telnum[SMS_SENDER_SIZE];
    size_t telnum_length;
    unsigned int timestamp[8];
} sms_t;

/*
 * @brief	Decode SMS from PDU string received from modem.
 * @param	data	SMS message in PDU format received from modem
 * @param	sz		Length of the data
 * @param	sms		Pointer to output sms_t structure
 * @return	int		Zero on success
 */
extern int sms_decode_pdu(const char *data, size_t sz, sms_t *sms);

/*
 * @brief	Create PDU string from input data.
 * @param	sms		Pointer to output sms_t structure
 * @param	data	Pointer to output buffer
 * @param	sz		Size of hte output buffer
 * @return	int		Nonnegative number of success
 *
 * This function encodes PDU message from input data stored in sms_t structure.
 * Please do not forget to correctly fill all fields in sms_t structure
 * passed to this function. On successful completion function returns length of
 * the encoded PDU string.
 */
extern int sms_encode_pdu(sms_t *sms, char *data, size_t sz);

/*
 * @brief	Write message to the SMS message.
 * @param	mesg	Zero terminated message string
 * @param	sms		Destination SMS
 * @return	int		Nonnegative number of success
 * 
 * Function stores message pointed by mesg pointer into sms message.
 * Useful in case you want to construct reply message to received sms. Just simply call
 * this function and use encoded PDU string.
 */
extern ssize_t sms_write(const char *mesg, sms_t *sms);

#endif	/* SMS_PDU_H */

