AirPDcapRsnaPwd2PskStep(
    const guint8 *ppBytes,
    const guint ppLength,
    const CHAR *ssid,
    const size_t ssidLength,
    const INT iterations,
    const INT count,
    UCHAR *output)
{
    UCHAR digest[MAX_SSID_LENGTH+4];  /* SSID plus 4 bytes of count */
    UCHAR digest1[SHA1_DIGEST_LEN];
    INT i, j;

    if (ssidLength > MAX_SSID_LENGTH) {
        /* This "should not happen" */
        return AIRPDCAP_RET_UNSUCCESS;
    }

    memset(digest, 0, sizeof digest);
    memset(digest1, 0, sizeof digest1);

    /* U1 = PRF(P, S || INT(i)) */
    memcpy(digest, ssid, ssidLength);
    digest[ssidLength] = (UCHAR)((count>>24) & 0xff);
    digest[ssidLength+1] = (UCHAR)((count>>16) & 0xff);
    digest[ssidLength+2] = (UCHAR)((count>>8) & 0xff);
    digest[ssidLength+3] = (UCHAR)(count & 0xff);
    sha1_hmac(ppBytes, ppLength, digest, (guint32) ssidLength+4, digest1);

    /* output = U1 */
    memcpy(output, digest1, SHA1_DIGEST_LEN);
    for (i = 1; i < iterations; i++) {
        /* Un = PRF(P, Un-1) */
        sha1_hmac(ppBytes, ppLength, digest1, SHA1_DIGEST_LEN, digest);

        memcpy(digest1, digest, SHA1_DIGEST_LEN);
        /* output = output xor Un */
        for (j = 0; j < SHA1_DIGEST_LEN; j++) {
            output[j] ^= digest[j];
        }
    }

    return AIRPDCAP_RET_SUCCESS;
}
