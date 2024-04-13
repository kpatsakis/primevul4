AirPDcapValidateKey(
    PAIRPDCAP_KEY_ITEM key)
{
    size_t len;
    UCHAR ret=TRUE;
    AIRPDCAP_DEBUG_TRACE_START("AirPDcapValidateKey");

    if (key==NULL) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapValidateKey", "NULL key", AIRPDCAP_DEBUG_LEVEL_5);
        AIRPDCAP_DEBUG_TRACE_START("AirPDcapValidateKey");
        return FALSE;
    }

    switch (key->KeyType) {
        case AIRPDCAP_KEY_TYPE_WEP:
            /* check key size limits */
            len=key->KeyData.Wep.WepKeyLen;
            if (len<AIRPDCAP_WEP_KEY_MINLEN || len>AIRPDCAP_WEP_KEY_MAXLEN) {
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapValidateKey", "WEP key: key length not accepted", AIRPDCAP_DEBUG_LEVEL_5);
                ret=FALSE;
            }
            break;

        case AIRPDCAP_KEY_TYPE_WEP_40:
            /* set the standard length and use a generic WEP key type */
            key->KeyData.Wep.WepKeyLen=AIRPDCAP_WEP_40_KEY_LEN;
            key->KeyType=AIRPDCAP_KEY_TYPE_WEP;
            break;

        case AIRPDCAP_KEY_TYPE_WEP_104:
            /* set the standard length and use a generic WEP key type */
            key->KeyData.Wep.WepKeyLen=AIRPDCAP_WEP_104_KEY_LEN;
            key->KeyType=AIRPDCAP_KEY_TYPE_WEP;
            break;

        case AIRPDCAP_KEY_TYPE_WPA_PWD:
            /* check passphrase and SSID size limits */
            len=strlen(key->UserPwd.Passphrase);
            if (len<AIRPDCAP_WPA_PASSPHRASE_MIN_LEN || len>AIRPDCAP_WPA_PASSPHRASE_MAX_LEN) {
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapValidateKey", "WPA-PWD key: passphrase length not accepted", AIRPDCAP_DEBUG_LEVEL_5);
                ret=FALSE;
            }

            len=key->UserPwd.SsidLen;
            if (len>AIRPDCAP_WPA_SSID_MAX_LEN) {
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapValidateKey", "WPA-PWD key: ssid length not accepted", AIRPDCAP_DEBUG_LEVEL_5);
                ret=FALSE;
            }

            break;

        case AIRPDCAP_KEY_TYPE_WPA_PSK:
            break;

        case AIRPDCAP_KEY_TYPE_WPA_PMK:
            break;

        default:
            ret=FALSE;
    }

    AIRPDCAP_DEBUG_TRACE_END("AirPDcapValidateKey");
    return ret;
}
