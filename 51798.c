AirPDcapRsnaMicCheck(
    UCHAR *eapol,
    USHORT eapol_len,
    UCHAR KCK[AIRPDCAP_WPA_KCK_LEN],
    USHORT key_ver)
{
    UCHAR mic[AIRPDCAP_WPA_MICKEY_LEN];
    UCHAR c_mic[20];  /* MIC 16 byte, the HMAC-SHA1 use a buffer of 20 bytes */

    /* copy the MIC from the EAPOL packet */
    memcpy(mic, eapol+AIRPDCAP_WPA_MICKEY_OFFSET+4, AIRPDCAP_WPA_MICKEY_LEN);

    /* set to 0 the MIC in the EAPOL packet (to calculate the MIC) */
    memset(eapol+AIRPDCAP_WPA_MICKEY_OFFSET+4, 0, AIRPDCAP_WPA_MICKEY_LEN);

    if (key_ver==AIRPDCAP_WPA_KEY_VER_NOT_CCMP) {
        /* use HMAC-MD5 for the EAPOL-Key MIC */
        md5_hmac(eapol, eapol_len, KCK, AIRPDCAP_WPA_KCK_LEN, c_mic);
    } else if (key_ver==AIRPDCAP_WPA_KEY_VER_AES_CCMP) {
        /* use HMAC-SHA1-128 for the EAPOL-Key MIC */
        sha1_hmac(KCK, AIRPDCAP_WPA_KCK_LEN, eapol, eapol_len, c_mic);
    } else
        /* key descriptor version not recognized */
        return AIRPDCAP_RET_UNSUCCESS;

    /* compare calculated MIC with the Key MIC and return result (0 means success) */
    return memcmp(mic, c_mic, AIRPDCAP_WPA_MICKEY_LEN);
}
