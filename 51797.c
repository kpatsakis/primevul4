AirPDcapRsna4WHandshake(
    PAIRPDCAP_CONTEXT ctx,
    const UCHAR *data,
    AIRPDCAP_SEC_ASSOCIATION *sa,
    INT offset,
    const guint tot_len)
{
    AIRPDCAP_KEY_ITEM *tmp_key, *tmp_pkt_key, pkt_key;
    AIRPDCAP_SEC_ASSOCIATION *tmp_sa;
    INT key_index;
    INT ret_value=1;
    UCHAR useCache=FALSE;
    UCHAR eapol[AIRPDCAP_EAPOL_MAX_LEN];
    USHORT eapol_len;

    if (sa->key!=NULL)
        useCache=TRUE;

    /* a 4-way handshake packet use a Pairwise key type (IEEE 802.11i-2004, pg. 79) */
    if (AIRPDCAP_EAP_KEY(data[offset+1])!=1) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "Group/STAKey message (not used)", AIRPDCAP_DEBUG_LEVEL_5);
        return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
    }

    /* TODO timeouts? */

    /* TODO consider key-index */

    /* TODO considera Deauthentications */

    AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "4-way handshake...", AIRPDCAP_DEBUG_LEVEL_5);

    /* manage 4-way handshake packets; this step completes the 802.1X authentication process (IEEE 802.11i-2004, pag. 85) */

    /* message 1: Authenticator->Supplicant (Sec=0, Mic=0, Ack=1, Inst=0, Key=1(pairwise), KeyRSC=0, Nonce=ANonce, MIC=0) */
    if (AIRPDCAP_EAP_INST(data[offset+1])==0 &&
        AIRPDCAP_EAP_ACK(data[offset+1])==1 &&
        AIRPDCAP_EAP_MIC(data[offset])==0)
    {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "4-way handshake message 1", AIRPDCAP_DEBUG_LEVEL_3);

        /* On reception of Message 1, the Supplicant determines whether the Key Replay Counter field value has been        */
        /* used before with the current PMKSA. If the Key Replay Counter field value is less than or equal to the current  */
        /* local value, the Supplicant discards the message.                                                               */
        /* -> not checked, the Authenticator will be send another Message 1 (hopefully!)                                   */

        /* This saves the sa since we are reauthenticating which will overwrite our current sa GCS*/
        if( sa->handshake >= 2) {
            tmp_sa= g_new(AIRPDCAP_SEC_ASSOCIATION, 1);
            memcpy(tmp_sa, sa, sizeof(AIRPDCAP_SEC_ASSOCIATION));
            sa->validKey=FALSE;
            sa->next=tmp_sa;
        }

        /* save ANonce (from authenticator) to derive the PTK with the SNonce (from the 2 message) */
        memcpy(sa->wpa.nonce, data+offset+12, 32);

        /* get the Key Descriptor Version (to select algorithm used in decryption -CCMP or TKIP-) */
        sa->wpa.key_ver=AIRPDCAP_EAP_KEY_DESCR_VER(data[offset+1]);

        sa->handshake=1;

        return AIRPDCAP_RET_SUCCESS_HANDSHAKE;
    }

    /* message 2|4: Supplicant->Authenticator (Sec=0|1, Mic=1, Ack=0, Inst=0, Key=1(pairwise), KeyRSC=0, Nonce=SNonce|0, MIC=MIC(KCK,EAPOL)) */
    if (AIRPDCAP_EAP_INST(data[offset+1])==0 &&
        AIRPDCAP_EAP_ACK(data[offset+1])==0 &&
        AIRPDCAP_EAP_MIC(data[offset])==1)
    {
        /* Check key data length to differentiate between message 2 or 4, same as in epan/dissectors/packet-ieee80211.c */
        if (pntoh16(data+offset+92)) {
            /* message 2 */
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "4-way handshake message 2", AIRPDCAP_DEBUG_LEVEL_3);

            /* On reception of Message 2, the Authenticator checks that the key replay counter corresponds to the */
            /* outstanding Message 1. If not, it silently discards the message.                                   */
            /* If the calculated MIC does not match the MIC that the Supplicant included in the EAPOL-Key frame,  */
            /* the Authenticator silently discards Message 2.                                                     */
            /* -> not checked; the Supplicant will send another message 2 (hopefully!)                            */

            /* now you can derive the PTK */
            for (key_index=0; key_index<(INT)ctx->keys_nr || useCache; key_index++) {
                /* use the cached one, or try all keys */
                if (!useCache) {
                    AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "Try WPA key...", AIRPDCAP_DEBUG_LEVEL_3);
                    tmp_key=&ctx->keys[key_index];
                } else {
                    /* there is a cached key in the security association, if it's a WPA key try it... */
                    if (sa->key!=NULL &&
                        (sa->key->KeyType==AIRPDCAP_KEY_TYPE_WPA_PWD ||
                         sa->key->KeyType==AIRPDCAP_KEY_TYPE_WPA_PSK ||
                         sa->key->KeyType==AIRPDCAP_KEY_TYPE_WPA_PMK)) {
                            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "Try cached WPA key...", AIRPDCAP_DEBUG_LEVEL_3);
                            tmp_key=sa->key;
                    } else {
                        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "Cached key is of a wrong type, try WPA key...", AIRPDCAP_DEBUG_LEVEL_3);
                        tmp_key=&ctx->keys[key_index];
                    }
                }

                /* obviously, try only WPA keys... */
                if (tmp_key->KeyType==AIRPDCAP_KEY_TYPE_WPA_PWD ||
                    tmp_key->KeyType==AIRPDCAP_KEY_TYPE_WPA_PSK ||
                    tmp_key->KeyType==AIRPDCAP_KEY_TYPE_WPA_PMK)
                {
                    if (tmp_key->KeyType == AIRPDCAP_KEY_TYPE_WPA_PWD && tmp_key->UserPwd.SsidLen == 0 && ctx->pkt_ssid_len > 0 && ctx->pkt_ssid_len <= AIRPDCAP_WPA_SSID_MAX_LEN) {
                        /* We have a "wildcard" SSID.  Use the one from the packet. */
                        memcpy(&pkt_key, tmp_key, sizeof(pkt_key));
                        memcpy(&pkt_key.UserPwd.Ssid, ctx->pkt_ssid, ctx->pkt_ssid_len);
                         pkt_key.UserPwd.SsidLen = ctx->pkt_ssid_len;
                        AirPDcapRsnaPwd2Psk(pkt_key.UserPwd.Passphrase, pkt_key.UserPwd.Ssid,
                            pkt_key.UserPwd.SsidLen, pkt_key.KeyData.Wpa.Psk);
                        tmp_pkt_key = &pkt_key;
                    } else {
                        tmp_pkt_key = tmp_key;
                    }

                    /* derive the PTK from the BSSID, STA MAC, PMK, SNonce, ANonce */
                    AirPDcapRsnaPrfX(sa,                            /* authenticator nonce, bssid, station mac */
                                     tmp_pkt_key->KeyData.Wpa.Psk,      /* PSK == PMK */
                                     data+offset+12,                /* supplicant nonce */
                                     512,
                                     sa->wpa.ptk);

                    /* verify the MIC (compare the MIC in the packet included in this message with a MIC calculated with the PTK) */
                    eapol_len=pntoh16(data+offset-3)+4;
                    memcpy(eapol, &data[offset-5], (eapol_len<AIRPDCAP_EAPOL_MAX_LEN?eapol_len:AIRPDCAP_EAPOL_MAX_LEN));
                    ret_value=AirPDcapRsnaMicCheck(eapol,           /*      eapol frame (header also) */
                                                   eapol_len,       /*      eapol frame length        */
                                                   sa->wpa.ptk,     /*      Key Confirmation Key      */
                                                   AIRPDCAP_EAP_KEY_DESCR_VER(data[offset+1])); /*  EAPOL-Key description version */

                    /* If the MIC is valid, the Authenticator checks that the RSN information element bit-wise matches       */
                    /* that from the (Re)Association Request message.                                                        */
                    /*              i) TODO If these are not exactly the same, the Authenticator uses MLME-DEAUTHENTICATE.request */
                    /* primitive to terminate the association.                                                               */
                    /*              ii) If they do match bit-wise, the Authenticator constructs Message 3.                   */
                }

                if (!ret_value &&
                    (tmp_key->KeyType==AIRPDCAP_KEY_TYPE_WPA_PWD ||
                    tmp_key->KeyType==AIRPDCAP_KEY_TYPE_WPA_PSK ||
                    tmp_key->KeyType==AIRPDCAP_KEY_TYPE_WPA_PMK))
                {
                    /* the temporary key is the correct one, cached in the Security Association */

                    sa->key=tmp_key;
                    break;
                } else {
                    /* the cached key was not valid, try other keys */

                    if (useCache==TRUE) {
                        useCache=FALSE;
                        key_index--;
                    }
                }
            }

            if (ret_value) {
                AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "handshake step failed", AIRPDCAP_DEBUG_LEVEL_3);
                return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
            }

            sa->handshake=2;
            sa->validKey=TRUE; /* we can use the key to decode, even if we have not captured the other eapol packets */

            return AIRPDCAP_RET_SUCCESS_HANDSHAKE;
        } else {
        /* message 4 */

            /* TODO "Note that when the 4-Way Handshake is first used Message 4 is sent in the clear." */

            /* TODO check MIC and Replay Counter                                                                     */
            /* On reception of Message 4, the Authenticator verifies that the Key Replay Counter field value is one  */
            /* that it used on this 4-Way Handshake; if it is not, it silently discards the message.                 */
            /* If the calculated MIC does not match the MIC that the Supplicant included in the EAPOL-Key frame, the */
            /* Authenticator silently discards Message 4.                                                            */

            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "4-way handshake message 4", AIRPDCAP_DEBUG_LEVEL_3);

            sa->handshake=4;

            return AIRPDCAP_RET_SUCCESS_HANDSHAKE;
        }
    }

    /* message 3: Authenticator->Supplicant (Sec=1, Mic=1, Ack=1, Inst=0/1, Key=1(pairwise), KeyRSC=???, Nonce=ANonce, MIC=1) */
    if (AIRPDCAP_EAP_ACK(data[offset+1])==1 &&
        AIRPDCAP_EAP_MIC(data[offset])==1)
    {
        const EAPOL_RSN_KEY *pEAPKey;
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapRsna4WHandshake", "4-way handshake message 3", AIRPDCAP_DEBUG_LEVEL_3);

        /* On reception of Message 3, the Supplicant silently discards the message if the Key Replay Counter field     */
        /* value has already been used or if the ANonce value in Message 3 differs from the ANonce value in Message 1. */
        /* -> not checked, the Authenticator will send another message 3 (hopefully!)                                  */

        /* TODO check page 88 (RNS) */

        /* If using WPA2 PSK, message 3 will contain an RSN for the group key (GTK KDE).
           In order to properly support decrypting WPA2-PSK packets, we need to parse this to get the group key. */
        pEAPKey = (const EAPOL_RSN_KEY *)(&(data[offset-1]));
        if (pEAPKey->type == AIRPDCAP_RSN_WPA2_KEY_DESCRIPTOR){
            PAIRPDCAP_SEC_ASSOCIATION broadcast_sa;
            AIRPDCAP_SEC_ASSOCIATION_ID id;

            /* Get broadcacst SA for the current BSSID */
            memcpy(id.sta, broadcast_mac, AIRPDCAP_MAC_LEN);
            memcpy(id.bssid, sa->saId.bssid, AIRPDCAP_MAC_LEN);
            broadcast_sa = AirPDcapGetSaPtr(ctx, &id);

            if (broadcast_sa == NULL){
                return AIRPDCAP_RET_REQ_DATA;
            }
            return (AirPDcapDecryptWPABroadcastKey(pEAPKey, sa->wpa.ptk+16, broadcast_sa, tot_len-offset+1));
        }
    }

    return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
}
