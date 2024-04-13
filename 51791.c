AirPDcapGetSaAddress(
    const AIRPDCAP_MAC_FRAME_ADDR4 *frame,
    AIRPDCAP_SEC_ASSOCIATION_ID *id)
{
#ifdef _DEBUG
#define MSGBUF_LEN 255
    CHAR msgbuf[MSGBUF_LEN];
#endif

    if ((AIRPDCAP_TYPE(frame->fc[0])==AIRPDCAP_TYPE_DATA) &&
        (AIRPDCAP_DS_BITS(frame->fc[1]) == 0) &&
        (memcmp(frame->addr2, frame->addr3, AIRPDCAP_MAC_LEN) != 0) &&
        (memcmp(frame->addr1, frame->addr3, AIRPDCAP_MAC_LEN) != 0)) {
        /* DATA frame with fromDS=0 ToDS=0 and neither RA or SA is BSSID
           => TDLS traffic. Use highest MAC address for bssid */
        if (memcmp(frame->addr1, frame->addr2, AIRPDCAP_MAC_LEN) < 0) {
            memcpy(id->sta, frame->addr1, AIRPDCAP_MAC_LEN);
            memcpy(id->bssid, frame->addr2, AIRPDCAP_MAC_LEN);
        } else {
            memcpy(id->sta, frame->addr2, AIRPDCAP_MAC_LEN);
            memcpy(id->bssid, frame->addr1, AIRPDCAP_MAC_LEN);
        }
    } else {
        const UCHAR *addr;

        /* Normal Case: SA between STA and AP */
        if ((addr = AirPDcapGetBssidAddress(frame)) != NULL) {
            memcpy(id->bssid, addr, AIRPDCAP_MAC_LEN);
        } else {
            return AIRPDCAP_RET_UNSUCCESS;
        }

        if ((addr = AirPDcapGetStaAddress(frame)) != NULL) {
            memcpy(id->sta, addr, AIRPDCAP_MAC_LEN);
        } else {
            return AIRPDCAP_RET_UNSUCCESS;
        }
    }

#ifdef _DEBUG
    g_snprintf(msgbuf, MSGBUF_LEN, "BSSID_MAC: %02X.%02X.%02X.%02X.%02X.%02X\t",
               id->bssid[0],id->bssid[1],id->bssid[2],id->bssid[3],id->bssid[4],id->bssid[5]);
    AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapGetSaAddress", msgbuf, AIRPDCAP_DEBUG_LEVEL_3);
    g_snprintf(msgbuf, MSGBUF_LEN, "STA_MAC: %02X.%02X.%02X.%02X.%02X.%02X\t",
               id->sta[0],id->sta[1],id->sta[2],id->sta[3],id->sta[4],id->sta[5]);
    AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapGetSaAddress", msgbuf, AIRPDCAP_DEBUG_LEVEL_3);
#endif

    return AIRPDCAP_RET_SUCCESS;
}
