AirPDcapRsnaPrfX(
    AIRPDCAP_SEC_ASSOCIATION *sa,
    const UCHAR pmk[32],
    const UCHAR snonce[32],
    const INT x,        /*      for TKIP 512, for CCMP 384 */
    UCHAR *ptk)
{
    UINT8 i;
    UCHAR R[100];
    INT offset=sizeof("Pairwise key expansion");
    UCHAR output[80]; /* allow for sha1 overflow. */

    memset(R, 0, 100);

    memcpy(R, "Pairwise key expansion", offset);

    /* Min(AA, SPA) || Max(AA, SPA) */
    if (memcmp(sa->saId.sta, sa->saId.bssid, AIRPDCAP_MAC_LEN) < 0)
    {
        memcpy(R + offset, sa->saId.sta, AIRPDCAP_MAC_LEN);
        memcpy(R + offset+AIRPDCAP_MAC_LEN, sa->saId.bssid, AIRPDCAP_MAC_LEN);
    }
    else
    {
        memcpy(R + offset, sa->saId.bssid, AIRPDCAP_MAC_LEN);
        memcpy(R + offset+AIRPDCAP_MAC_LEN, sa->saId.sta, AIRPDCAP_MAC_LEN);
    }

    offset+=AIRPDCAP_MAC_LEN*2;

    /* Min(ANonce,SNonce) || Max(ANonce,SNonce) */
    if( memcmp(snonce, sa->wpa.nonce, 32) < 0 )
    {
        memcpy(R + offset, snonce, 32);
        memcpy(R + offset + 32, sa->wpa.nonce, 32);
    }
    else
    {
        memcpy(R + offset, sa->wpa.nonce, 32);
        memcpy(R + offset + 32, snonce, 32);
    }

    offset+=32*2;

    for(i = 0; i < (x+159)/160; i++)
    {
        R[offset] = i;
        sha1_hmac(pmk, 32, R, 100, &output[20 * i]);
    }
    memcpy(ptk, output, x/8);
}
