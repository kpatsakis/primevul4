AirPDcapTDLSDeriveKey(
    PAIRPDCAP_SEC_ASSOCIATION sa,
    const guint8 *data,
    guint offset_rsne,
    guint offset_fte,
    guint offset_timeout,
    guint offset_link,
    guint8 action)
{

    sha256_hmac_context sha_ctx;
    aes_cmac_ctx aes_ctx;
    const guint8 *snonce, *anonce, *initiator, *responder, *bssid;
    guint8 key_input[SHA256_DIGEST_LEN];
    guint8 mic[16], iter[2], length[2], seq_num = action + 1;

    /* Get key input */
    anonce = &data[offset_fte + 20];
    snonce = &data[offset_fte + 52];
    sha256_starts(&(sha_ctx.ctx));
    if (memcmp(anonce, snonce, AIRPDCAP_WPA_NONCE_LEN) < 0) {
        sha256_update(&(sha_ctx.ctx), anonce, AIRPDCAP_WPA_NONCE_LEN);
        sha256_update(&(sha_ctx.ctx), snonce, AIRPDCAP_WPA_NONCE_LEN);
    } else {
        sha256_update(&(sha_ctx.ctx), snonce, AIRPDCAP_WPA_NONCE_LEN);
        sha256_update(&(sha_ctx.ctx), anonce, AIRPDCAP_WPA_NONCE_LEN);
    }
    sha256_finish(&(sha_ctx.ctx), key_input);

    /* Derive key */
    bssid = &data[offset_link + 2];
    initiator = &data[offset_link + 8];
    responder = &data[offset_link + 14];
    sha256_hmac_starts(&sha_ctx, key_input, SHA256_DIGEST_LEN);
    iter[0] = 1;
    iter[1] = 0;
    sha256_hmac_update(&sha_ctx, (const guint8 *)&iter, 2);
    sha256_hmac_update(&sha_ctx, "TDLS PMK", 8);
    if (memcmp(initiator, responder, AIRPDCAP_MAC_LEN) < 0) {
        sha256_hmac_update(&sha_ctx, initiator, AIRPDCAP_MAC_LEN);
        sha256_hmac_update(&sha_ctx, responder, AIRPDCAP_MAC_LEN);
    } else {
        sha256_hmac_update(&sha_ctx, responder, AIRPDCAP_MAC_LEN);
        sha256_hmac_update(&sha_ctx, initiator, AIRPDCAP_MAC_LEN);
    }
    sha256_hmac_update(&sha_ctx, bssid, AIRPDCAP_MAC_LEN);
    length[0] = 256 & 0xff;
    length[1] = (256 >> 8) & 0xff;
    sha256_hmac_update(&sha_ctx, (const guint8 *)&length, 2);
    sha256_hmac_finish(&sha_ctx, key_input);

    /* Check MIC */
    aes_cmac_encrypt_starts(&aes_ctx, key_input, 16);
    aes_cmac_encrypt_update(&aes_ctx, initiator, AIRPDCAP_MAC_LEN);
    aes_cmac_encrypt_update(&aes_ctx, responder, AIRPDCAP_MAC_LEN);
    aes_cmac_encrypt_update(&aes_ctx, &seq_num, 1);
    aes_cmac_encrypt_update(&aes_ctx, &data[offset_link], data[offset_link + 1] + 2);
    aes_cmac_encrypt_update(&aes_ctx, &data[offset_rsne], data[offset_rsne + 1] + 2);
    aes_cmac_encrypt_update(&aes_ctx, &data[offset_timeout], data[offset_timeout + 1] + 2);
    aes_cmac_encrypt_update(&aes_ctx, &data[offset_fte], 4);
    memset(mic, 0, 16);
    aes_cmac_encrypt_update(&aes_ctx, mic, 16);
    aes_cmac_encrypt_update(&aes_ctx, &data[offset_fte + 20], data[offset_fte + 1] + 2 - 20);
    aes_cmac_encrypt_finish(&aes_ctx, mic);

    if (memcmp(mic, &data[offset_fte + 4],16)) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapTDLSDeriveKey", "MIC verification failed", AIRPDCAP_DEBUG_LEVEL_3);
        return AIRPDCAP_RET_UNSUCCESS;
    }

    memcpy(AIRPDCAP_GET_TK(sa->wpa.ptk), &key_input[16], 16);
    memcpy(sa->wpa.nonce, snonce, AIRPDCAP_WPA_NONCE_LEN);
    sa->validKey = TRUE;
    sa->wpa.key_ver = AIRPDCAP_WPA_KEY_VER_AES_CCMP;
    AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapTDLSDeriveKey", "MIC verified", AIRPDCAP_DEBUG_LEVEL_3);
    return  AIRPDCAP_RET_SUCCESS;
}
