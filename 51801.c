AirPDcapRsnaPwd2Psk(
    const CHAR *passphrase,
    const CHAR *ssid,
    const size_t ssidLength,
    UCHAR *output)
{
    UCHAR m_output[2*SHA1_DIGEST_LEN];
    GByteArray *pp_ba = g_byte_array_new();

    memset(m_output, 0, 2*SHA1_DIGEST_LEN);

    if (!uri_str_to_bytes(passphrase, pp_ba)) {
        g_byte_array_free(pp_ba, TRUE);
        return 0;
    }

    AirPDcapRsnaPwd2PskStep(pp_ba->data, pp_ba->len, ssid, ssidLength, 4096, 1, m_output);
    AirPDcapRsnaPwd2PskStep(pp_ba->data, pp_ba->len, ssid, ssidLength, 4096, 2, &m_output[SHA1_DIGEST_LEN]);

    memcpy(output, m_output, AIRPDCAP_WPA_PSK_LEN);
    g_byte_array_free(pp_ba, TRUE);

    return 0;
}
