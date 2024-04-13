INT AirPDcapSetKeys(
    PAIRPDCAP_CONTEXT ctx,
    AIRPDCAP_KEY_ITEM keys[],
    const size_t keys_nr)
{
    INT i;
    INT success;
    AIRPDCAP_DEBUG_TRACE_START("AirPDcapSetKeys");

    if (ctx==NULL || keys==NULL) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapSetKeys", "NULL context or NULL keys array", AIRPDCAP_DEBUG_LEVEL_3);
        AIRPDCAP_DEBUG_TRACE_END("AirPDcapSetKeys");
        return 0;
    }

    if (keys_nr>AIRPDCAP_MAX_KEYS_NR) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapSetKeys", "Keys number greater than maximum", AIRPDCAP_DEBUG_LEVEL_3);
        AIRPDCAP_DEBUG_TRACE_END("AirPDcapSetKeys");
        return 0;
    }

    /* clean key and SA collections before setting new ones */
    AirPDcapInitContext(ctx);

    /* check and insert keys */
    for (i=0, success=0; i<(INT)keys_nr; i++) {
        if (AirPDcapValidateKey(keys+i)==TRUE) {
            if (keys[i].KeyType==AIRPDCAP_KEY_TYPE_WPA_PWD) {
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapSetKeys", "Set a WPA-PWD key", AIRPDCAP_DEBUG_LEVEL_4);
                AirPDcapRsnaPwd2Psk(keys[i].UserPwd.Passphrase, keys[i].UserPwd.Ssid, keys[i].UserPwd.SsidLen, keys[i].KeyData.Wpa.Psk);
            }
#ifdef _DEBUG
            else if (keys[i].KeyType==AIRPDCAP_KEY_TYPE_WPA_PMK) {
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapSetKeys", "Set a WPA-PMK key", AIRPDCAP_DEBUG_LEVEL_4);
            } else if (keys[i].KeyType==AIRPDCAP_KEY_TYPE_WEP) {
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapSetKeys", "Set a WEP key", AIRPDCAP_DEBUG_LEVEL_4);
            } else {
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapSetKeys", "Set a key", AIRPDCAP_DEBUG_LEVEL_4);
            }
#endif
            memcpy(&ctx->keys[success], &keys[i], sizeof(keys[i]));
            success++;
        }
    }

    ctx->keys_nr=success;

    AIRPDCAP_DEBUG_TRACE_END("AirPDcapSetKeys");
    return success;
}
