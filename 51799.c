AirPDcapRsnaMng(
    UCHAR *decrypt_data,
    guint mac_header_len,
    guint *decrypt_len,
    PAIRPDCAP_KEY_ITEM key,
    AIRPDCAP_SEC_ASSOCIATION *sa,
    INT offset)
{
    INT ret_value=1;
    UCHAR *try_data;
    guint try_data_len = *decrypt_len;

    if (*decrypt_len > try_data_len) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsnaMng", "Invalid decryption length", AIRPDCAP_DEBUG_LEVEL_3);
        return AIRPDCAP_RET_UNSUCCESS;
    }

    /* allocate a temp buffer for the decryption loop */
    try_data=(UCHAR *)g_malloc(try_data_len);

    /* start of loop added by GCS */
    for(/* sa */; sa != NULL ;sa=sa->next) {

       if (sa->validKey==FALSE) {
           AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsnaMng", "Key not yet valid", AIRPDCAP_DEBUG_LEVEL_3);
           continue;
       }

       /* copy the encrypted data into a temp buffer */
       memcpy(try_data, decrypt_data, *decrypt_len);

       if (sa->wpa.key_ver==1) {
           /* CCMP -> HMAC-MD5 is the EAPOL-Key MIC, RC4 is the EAPOL-Key encryption algorithm */
           AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsnaMng", "TKIP", AIRPDCAP_DEBUG_LEVEL_3);
           DEBUG_DUMP("ptk", sa->wpa.ptk, 64);
           DEBUG_DUMP("ptk portion used", AIRPDCAP_GET_TK(sa->wpa.ptk), 16);

           ret_value=AirPDcapTkipDecrypt(try_data+offset, *decrypt_len-offset, try_data+AIRPDCAP_TA_OFFSET, AIRPDCAP_GET_TK(sa->wpa.ptk));
           if (ret_value){
               AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsnaMng", "TKIP failed!", AIRPDCAP_DEBUG_LEVEL_3);
               continue;
           }

           AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsnaMng", "TKIP DECRYPTED!!!", AIRPDCAP_DEBUG_LEVEL_3);
           /* remove MIC (8bytes) and ICV (4bytes) from the end of packet */
           *decrypt_len-=12;
           break;
       } else {
           /* AES-CCMP -> HMAC-SHA1-128 is the EAPOL-Key MIC, AES wep_key wrap is the EAPOL-Key encryption algorithm */
           AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsnaMng", "CCMP", AIRPDCAP_DEBUG_LEVEL_3);

           ret_value=AirPDcapCcmpDecrypt(try_data, mac_header_len, (INT)*decrypt_len, AIRPDCAP_GET_TK(sa->wpa.ptk));
           if (ret_value)
              continue;

           AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsnaMng", "CCMP DECRYPTED!!!", AIRPDCAP_DEBUG_LEVEL_3);
           /* remove MIC (8bytes) from the end of packet */
           *decrypt_len-=8;
           break;
       }
    }
    /* end of loop */

    /* none of the keys worked */
    if(sa == NULL) {
        g_free(try_data);
        return ret_value;
    }

    if (*decrypt_len > try_data_len || *decrypt_len < 8) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsnaMng", "Invalid decryption length", AIRPDCAP_DEBUG_LEVEL_3);
        g_free(try_data);
        return AIRPDCAP_RET_UNSUCCESS;
    }

    /* copy the decrypted data into the decrypt buffer GCS*/
    memcpy(decrypt_data, try_data, *decrypt_len);
    g_free(try_data);

    /* remove protection bit */
    decrypt_data[1]&=0xBF;

    /* remove TKIP/CCMP header */
    offset = mac_header_len;
    *decrypt_len-=8;
    memmove(decrypt_data+offset, decrypt_data+offset+8, *decrypt_len-offset);

    if (key!=NULL) {
        if (sa->key!=NULL)
            memcpy(key, sa->key, sizeof(AIRPDCAP_KEY_ITEM));
        else
            memset(key, 0, sizeof(AIRPDCAP_KEY_ITEM));
        memcpy(key->KeyData.Wpa.Ptk, sa->wpa.ptk, AIRPDCAP_WPA_PTK_LEN); /* copy the PTK to the key structure for future use by wireshark */
        if (sa->wpa.key_ver==AIRPDCAP_WPA_KEY_VER_NOT_CCMP)
            key->KeyType=AIRPDCAP_KEY_TYPE_TKIP;
        else if (sa->wpa.key_ver==AIRPDCAP_WPA_KEY_VER_AES_CCMP)
            key->KeyType=AIRPDCAP_KEY_TYPE_CCMP;
    }

    return AIRPDCAP_RET_SUCCESS;
}
