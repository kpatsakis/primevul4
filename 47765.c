int ssl3_send_client_key_exchange(SSL *s)
{
    unsigned char *p;
    int n;
    unsigned long alg_k;
#ifndef OPENSSL_NO_RSA
    unsigned char *q;
    EVP_PKEY *pkey = NULL;
#endif
#ifndef OPENSSL_NO_EC
    EC_KEY *clnt_ecdh = NULL;
    const EC_POINT *srvr_ecpoint = NULL;
    EVP_PKEY *srvr_pub_pkey = NULL;
    unsigned char *encodedPoint = NULL;
    int encoded_pt_len = 0;
    BN_CTX *bn_ctx = NULL;
#endif
    unsigned char *pms = NULL;
    size_t pmslen = 0;

    if (s->state == SSL3_ST_CW_KEY_EXCH_A) {
        p = ssl_handshake_start(s);

        alg_k = s->s3->tmp.new_cipher->algorithm_mkey;

        /* Fool emacs indentation */
        if (0) {
        }
#ifndef OPENSSL_NO_RSA
        else if (alg_k & SSL_kRSA) {
            RSA *rsa;
            pmslen = SSL_MAX_MASTER_KEY_LENGTH;
            pms = OPENSSL_malloc(pmslen);
            if (!pms)
                goto memerr;

            if (s->session->sess_cert == NULL) {
                /*
                 * We should always have a server certificate with SSL_kRSA.
                 */
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                       ERR_R_INTERNAL_ERROR);
                goto err;
            }

            if (s->session->sess_cert->peer_rsa_tmp != NULL)
                rsa = s->session->sess_cert->peer_rsa_tmp;
            else {
                pkey =
                    X509_get_pubkey(s->session->
                                    sess_cert->peer_pkeys[SSL_PKEY_RSA_ENC].
                                    x509);
                if ((pkey == NULL) || (pkey->type != EVP_PKEY_RSA)
                    || (pkey->pkey.rsa == NULL)) {
                    SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                           ERR_R_INTERNAL_ERROR);
                    goto err;
                }
                rsa = pkey->pkey.rsa;
                EVP_PKEY_free(pkey);
            }

            pms[0] = s->client_version >> 8;
            pms[1] = s->client_version & 0xff;
            if (RAND_bytes(pms + 2, pmslen - 2) <= 0)
                goto err;

            q = p;
            /* Fix buf for TLS and beyond */
            if (s->version > SSL3_VERSION)
                p += 2;
            n = RSA_public_encrypt(pmslen, pms, p, rsa, RSA_PKCS1_PADDING);
# ifdef PKCS1_CHECK
            if (s->options & SSL_OP_PKCS1_CHECK_1)
                p[1]++;
            if (s->options & SSL_OP_PKCS1_CHECK_2)
                tmp_buf[0] = 0x70;
# endif
            if (n <= 0) {
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                       SSL_R_BAD_RSA_ENCRYPT);
                goto err;
            }

            /* Fix buf for TLS and beyond */
            if (s->version > SSL3_VERSION) {
                s2n(n, q);
                n += 2;
            }
        }
#endif
#ifndef OPENSSL_NO_DH
        else if (alg_k & (SSL_kDHE | SSL_kDHr | SSL_kDHd)) {
            DH *dh_srvr, *dh_clnt;
            SESS_CERT *scert = s->session->sess_cert;

            if (scert == NULL) {
                ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_UNEXPECTED_MESSAGE);
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                       SSL_R_UNEXPECTED_MESSAGE);
                goto err;
            }

            if (scert->peer_dh_tmp != NULL)
                dh_srvr = scert->peer_dh_tmp;
            else {
                /* we get them from the cert */
                int idx = scert->peer_cert_type;
                EVP_PKEY *spkey = NULL;
                dh_srvr = NULL;
                if (idx >= 0)
                    spkey = X509_get_pubkey(scert->peer_pkeys[idx].x509);
                if (spkey) {
                    dh_srvr = EVP_PKEY_get1_DH(spkey);
                    EVP_PKEY_free(spkey);
                }
                if (dh_srvr == NULL) {
                    SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                           ERR_R_INTERNAL_ERROR);
                    goto err;
                }
            }
            if (s->s3->flags & TLS1_FLAGS_SKIP_CERT_VERIFY) {
                /* Use client certificate key */
                EVP_PKEY *clkey = s->cert->key->privatekey;
                dh_clnt = NULL;
                if (clkey)
                    dh_clnt = EVP_PKEY_get1_DH(clkey);
                if (dh_clnt == NULL) {
                    SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                           ERR_R_INTERNAL_ERROR);
                    goto err;
                }
            } else {
                /* generate a new random key */
                if ((dh_clnt = DHparams_dup(dh_srvr)) == NULL) {
                    SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE, ERR_R_DH_LIB);
                    goto err;
                }
                if (!DH_generate_key(dh_clnt)) {
                    SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE, ERR_R_DH_LIB);
                    DH_free(dh_clnt);
                    goto err;
                }
            }

            pmslen = DH_size(dh_clnt);
            pms = OPENSSL_malloc(pmslen);
            if (!pms)
                goto memerr;

            /*
             * use the 'p' output buffer for the DH key, but make sure to
             * clear it out afterwards
             */

            n = DH_compute_key(pms, dh_srvr->pub_key, dh_clnt);
            if (scert->peer_dh_tmp == NULL)
                DH_free(dh_srvr);

            if (n <= 0) {
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE, ERR_R_DH_LIB);
                DH_free(dh_clnt);
                goto err;
            }
            pmslen = n;

            if (s->s3->flags & TLS1_FLAGS_SKIP_CERT_VERIFY)
                n = 0;
            else {
                /* send off the data */
                n = BN_num_bytes(dh_clnt->pub_key);
                s2n(n, p);
                BN_bn2bin(dh_clnt->pub_key, p);
                n += 2;
            }

            DH_free(dh_clnt);

            /* perhaps clean things up a bit EAY EAY EAY EAY */
        }
#endif

#ifndef OPENSSL_NO_EC
        else if (alg_k & (SSL_kECDHE | SSL_kECDHr | SSL_kECDHe)) {
            const EC_GROUP *srvr_group = NULL;
            EC_KEY *tkey;
            int ecdh_clnt_cert = 0;
            int field_size = 0;

            if (s->session->sess_cert == NULL) {
                ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_UNEXPECTED_MESSAGE);
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                       SSL_R_UNEXPECTED_MESSAGE);
                goto err;
            }

            /*
             * Did we send out the client's ECDH share for use in premaster
             * computation as part of client certificate? If so, set
             * ecdh_clnt_cert to 1.
             */
            if ((alg_k & (SSL_kECDHr | SSL_kECDHe)) && (s->cert != NULL)) {
                /*-
                 * XXX: For now, we do not support client
                 * authentication using ECDH certificates.
                 * To add such support, one needs to add
                 * code that checks for appropriate
                 * conditions and sets ecdh_clnt_cert to 1.
                 * For example, the cert have an ECC
                 * key on the same curve as the server's
                 * and the key should be authorized for
                 * key agreement.
                 *
                 * One also needs to add code in ssl3_connect
                 * to skip sending the certificate verify
                 * message.
                 *
                 * if ((s->cert->key->privatekey != NULL) &&
                 *     (s->cert->key->privatekey->type ==
                 *      EVP_PKEY_EC) && ...)
                 * ecdh_clnt_cert = 1;
                 */
            }

            if (s->session->sess_cert->peer_ecdh_tmp != NULL) {
                tkey = s->session->sess_cert->peer_ecdh_tmp;
            } else {
                /* Get the Server Public Key from Cert */
                srvr_pub_pkey =
                    X509_get_pubkey(s->session->
                                    sess_cert->peer_pkeys[SSL_PKEY_ECC].x509);
                if ((srvr_pub_pkey == NULL)
                    || (srvr_pub_pkey->type != EVP_PKEY_EC)
                    || (srvr_pub_pkey->pkey.ec == NULL)) {
                    SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                           ERR_R_INTERNAL_ERROR);
                    goto err;
                }

                tkey = srvr_pub_pkey->pkey.ec;
            }

            srvr_group = EC_KEY_get0_group(tkey);
            srvr_ecpoint = EC_KEY_get0_public_key(tkey);

            if ((srvr_group == NULL) || (srvr_ecpoint == NULL)) {
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                       ERR_R_INTERNAL_ERROR);
                goto err;
            }

            if ((clnt_ecdh = EC_KEY_new()) == NULL) {
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                       ERR_R_MALLOC_FAILURE);
                goto err;
            }

            if (!EC_KEY_set_group(clnt_ecdh, srvr_group)) {
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE, ERR_R_EC_LIB);
                goto err;
            }
            if (ecdh_clnt_cert) {
                /*
                 * Reuse key info from our certificate We only need our
                 * private key to perform the ECDH computation.
                 */
                const BIGNUM *priv_key;
                tkey = s->cert->key->privatekey->pkey.ec;
                priv_key = EC_KEY_get0_private_key(tkey);
                if (priv_key == NULL) {
                    SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                           ERR_R_MALLOC_FAILURE);
                    goto err;
                }
                if (!EC_KEY_set_private_key(clnt_ecdh, priv_key)) {
                    SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE, ERR_R_EC_LIB);
                    goto err;
                }
            } else {
                /* Generate a new ECDH key pair */
                if (!(EC_KEY_generate_key(clnt_ecdh))) {
                    SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                           ERR_R_ECDH_LIB);
                    goto err;
                }
            }

            /*
             * use the 'p' output buffer for the ECDH key, but make sure to
             * clear it out afterwards
             */

            field_size = EC_GROUP_get_degree(srvr_group);
            if (field_size <= 0) {
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE, ERR_R_ECDH_LIB);
                goto err;
            }
            pmslen = (field_size + 7) / 8;
            pms = OPENSSL_malloc(pmslen);
            if (!pms)
                goto memerr;
            n = ECDH_compute_key(pms, pmslen, srvr_ecpoint, clnt_ecdh, NULL);
            if (n <= 0 || pmslen != (size_t)n) {
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE, ERR_R_ECDH_LIB);
                goto err;
            }

            if (ecdh_clnt_cert) {
                /* Send empty client key exch message */
                n = 0;
            } else {
                /*
                 * First check the size of encoding and allocate memory
                 * accordingly.
                 */
                encoded_pt_len =
                    EC_POINT_point2oct(srvr_group,
                                       EC_KEY_get0_public_key(clnt_ecdh),
                                       POINT_CONVERSION_UNCOMPRESSED,
                                       NULL, 0, NULL);

                encodedPoint = (unsigned char *)
                    OPENSSL_malloc(encoded_pt_len * sizeof(unsigned char));
                bn_ctx = BN_CTX_new();
                if ((encodedPoint == NULL) || (bn_ctx == NULL)) {
                    SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                           ERR_R_MALLOC_FAILURE);
                    goto err;
                }

                /* Encode the public key */
                n = EC_POINT_point2oct(srvr_group,
                                       EC_KEY_get0_public_key(clnt_ecdh),
                                       POINT_CONVERSION_UNCOMPRESSED,
                                       encodedPoint, encoded_pt_len, bn_ctx);

                *p = n;         /* length of encoded point */
                /* Encoded point will be copied here */
                p += 1;
                /* copy the point */
                memcpy(p, encodedPoint, n);
                /* increment n to account for length field */
                n += 1;
            }

            /* Free allocated memory */
            BN_CTX_free(bn_ctx);
            OPENSSL_free(encodedPoint);
            EC_KEY_free(clnt_ecdh);
            EVP_PKEY_free(srvr_pub_pkey);
        }
#endif                          /* !OPENSSL_NO_EC */
        else if (alg_k & SSL_kGOST) {
            /* GOST key exchange message creation */
            EVP_PKEY_CTX *pkey_ctx;
            X509 *peer_cert;
            size_t msglen;
            unsigned int md_len;
            int keytype;
            unsigned char shared_ukm[32], tmp[256];
            EVP_MD_CTX *ukm_hash;
            EVP_PKEY *pub_key;

            pmslen = 32;
            pms = OPENSSL_malloc(pmslen);
            if (!pms)
                goto memerr;

            /*
             * Get server sertificate PKEY and create ctx from it
             */
            peer_cert =
                s->session->
                sess_cert->peer_pkeys[(keytype = SSL_PKEY_GOST01)].x509;
            if (!peer_cert)
                peer_cert =
                    s->session->
                    sess_cert->peer_pkeys[(keytype = SSL_PKEY_GOST94)].x509;
            if (!peer_cert) {
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                       SSL_R_NO_GOST_CERTIFICATE_SENT_BY_PEER);
                goto err;
            }

            pkey_ctx = EVP_PKEY_CTX_new(pub_key =
                                        X509_get_pubkey(peer_cert), NULL);
            /*
             * If we have send a certificate, and certificate key
             *
             * * parameters match those of server certificate, use
             * certificate key for key exchange
             */

            /* Otherwise, generate ephemeral key pair */

            EVP_PKEY_encrypt_init(pkey_ctx);
            /* Generate session key */
            if (RAND_bytes(pms, pmslen) <= 0) {
                EVP_PKEY_CTX_free(pkey_ctx);
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                       ERR_R_INTERNAL_ERROR);
                goto err;
            };
            /*
             * If we have client certificate, use its secret as peer key
             */
            if (s->s3->tmp.cert_req && s->cert->key->privatekey) {
                if (EVP_PKEY_derive_set_peer
                    (pkey_ctx, s->cert->key->privatekey) <= 0) {
                    /*
                     * If there was an error - just ignore it. Ephemeral key
                     * * would be used
                     */
                    ERR_clear_error();
                }
            }
            /*
             * Compute shared IV and store it in algorithm-specific context
             * data
             */
            ukm_hash = EVP_MD_CTX_create();
            EVP_DigestInit(ukm_hash,
                           EVP_get_digestbynid(NID_id_GostR3411_94));
            EVP_DigestUpdate(ukm_hash, s->s3->client_random,
                             SSL3_RANDOM_SIZE);
            EVP_DigestUpdate(ukm_hash, s->s3->server_random,
                             SSL3_RANDOM_SIZE);
            EVP_DigestFinal_ex(ukm_hash, shared_ukm, &md_len);
            EVP_MD_CTX_destroy(ukm_hash);
            if (EVP_PKEY_CTX_ctrl
                (pkey_ctx, -1, EVP_PKEY_OP_ENCRYPT, EVP_PKEY_CTRL_SET_IV, 8,
                 shared_ukm) < 0) {
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                       SSL_R_LIBRARY_BUG);
                goto err;
            }
            /* Make GOST keytransport blob message */
            /*
             * Encapsulate it into sequence
             */
            *(p++) = V_ASN1_SEQUENCE | V_ASN1_CONSTRUCTED;
            msglen = 255;
            if (EVP_PKEY_encrypt(pkey_ctx, tmp, &msglen, pms, pmslen) < 0) {
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                       SSL_R_LIBRARY_BUG);
                goto err;
            }
            if (msglen >= 0x80) {
                *(p++) = 0x81;
                *(p++) = msglen & 0xff;
                n = msglen + 3;
            } else {
                *(p++) = msglen & 0xff;
                n = msglen + 2;
            }
            memcpy(p, tmp, msglen);
            /* Check if pubkey from client certificate was used */
            if (EVP_PKEY_CTX_ctrl
                (pkey_ctx, -1, -1, EVP_PKEY_CTRL_PEER_KEY, 2, NULL) > 0) {
                /* Set flag "skip certificate verify" */
                s->s3->flags |= TLS1_FLAGS_SKIP_CERT_VERIFY;
            }
            EVP_PKEY_CTX_free(pkey_ctx);
            EVP_PKEY_free(pub_key);

        }
#ifndef OPENSSL_NO_SRP
        else if (alg_k & SSL_kSRP) {
            if (s->srp_ctx.A != NULL) {
                /* send off the data */
                n = BN_num_bytes(s->srp_ctx.A);
                s2n(n, p);
                BN_bn2bin(s->srp_ctx.A, p);
                n += 2;
            } else {
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                       ERR_R_INTERNAL_ERROR);
                goto err;
            }
            OPENSSL_free(s->session->srp_username);
            s->session->srp_username = BUF_strdup(s->srp_ctx.login);
            if (s->session->srp_username == NULL) {
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                       ERR_R_MALLOC_FAILURE);
                goto err;
            }
        }
#endif
#ifndef OPENSSL_NO_PSK
        else if (alg_k & SSL_kPSK) {
            /*
             * The callback needs PSK_MAX_IDENTITY_LEN + 1 bytes to return a
             * \0-terminated identity. The last byte is for us for simulating
             * strnlen.
             */
            char identity[PSK_MAX_IDENTITY_LEN + 2];
            size_t identity_len;
            unsigned char *t = NULL;
            unsigned int psk_len = 0;
            int psk_err = 1;

            n = 0;
            if (s->psk_client_callback == NULL) {
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                       SSL_R_PSK_NO_CLIENT_CB);
                goto err;
            }

            memset(identity, 0, sizeof(identity));
            /* Allocate maximum size buffer */
            pmslen = PSK_MAX_PSK_LEN * 2 + 4;
            pms = OPENSSL_malloc(pmslen);
            if (!pms)
                goto memerr;

            psk_len = s->psk_client_callback(s, s->ctx->psk_identity_hint,
                                             identity, sizeof(identity) - 1,
                                             pms, pmslen);
            if (psk_len > PSK_MAX_PSK_LEN) {
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                       ERR_R_INTERNAL_ERROR);
                goto psk_err;
            } else if (psk_len == 0) {
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                       SSL_R_PSK_IDENTITY_NOT_FOUND);
                goto psk_err;
            }
            /* Change pmslen to real length */
            pmslen = 2 + psk_len + 2 + psk_len;
            identity[PSK_MAX_IDENTITY_LEN + 1] = '\0';
            identity_len = strlen(identity);
            if (identity_len > PSK_MAX_IDENTITY_LEN) {
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                       ERR_R_INTERNAL_ERROR);
                goto psk_err;
            }
            /* create PSK pre_master_secret */
            t = pms;
            memmove(pms + psk_len + 4, pms, psk_len);
            s2n(psk_len, t);
            memset(t, 0, psk_len);
            t += psk_len;
            s2n(psk_len, t);

            OPENSSL_free(s->session->psk_identity_hint);
            s->session->psk_identity_hint =
                BUF_strdup(s->ctx->psk_identity_hint);
            if (s->ctx->psk_identity_hint != NULL
                && s->session->psk_identity_hint == NULL) {
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                       ERR_R_MALLOC_FAILURE);
                goto psk_err;
            }

            OPENSSL_free(s->session->psk_identity);
            s->session->psk_identity = BUF_strdup(identity);
            if (s->session->psk_identity == NULL) {
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                       ERR_R_MALLOC_FAILURE);
                goto psk_err;
            }

            s2n(identity_len, p);
            memcpy(p, identity, identity_len);
            n = 2 + identity_len;
            psk_err = 0;
 psk_err:
            OPENSSL_cleanse(identity, sizeof(identity));
            if (psk_err != 0) {
                ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_HANDSHAKE_FAILURE);
                goto err;
            }
        }
#endif
        else {
            ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_HANDSHAKE_FAILURE);
            SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE, ERR_R_INTERNAL_ERROR);
            goto err;
        }

        if (!ssl_set_handshake_header(s, SSL3_MT_CLIENT_KEY_EXCHANGE, n)) {
            ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_HANDSHAKE_FAILURE);
            SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE, ERR_R_INTERNAL_ERROR);
            goto err;
        }

        s->state = SSL3_ST_CW_KEY_EXCH_B;
    }

    /* SSL3_ST_CW_KEY_EXCH_B */
    n = ssl_do_write(s);
#ifndef OPENSSL_NO_SRP
    /* Check for SRP */
    if (s->s3->tmp.new_cipher->algorithm_mkey & SSL_kSRP) {
        /*
         * If everything written generate master key: no need to save PMS as
         * SRP_generate_client_master_secret generates it internally.
         */
        if (n > 0) {
            if ((s->session->master_key_length =
                 SRP_generate_client_master_secret(s,
                                                   s->session->master_key)) <
                0) {
                SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
                       ERR_R_INTERNAL_ERROR);
                goto err;
            }
        }
    } else
#endif
        /* If we haven't written everything save PMS */
    if (n <= 0) {
        s->s3->tmp.pms = pms;
        s->s3->tmp.pmslen = pmslen;
    } else {
        /* If we don't have a PMS restore */
        if (pms == NULL) {
            pms = s->s3->tmp.pms;
            pmslen = s->s3->tmp.pmslen;
        }
        if (pms == NULL) {
            ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_INTERNAL_ERROR);
            SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE, ERR_R_MALLOC_FAILURE);
            goto err;
        }
        s->session->master_key_length =
            s->method->ssl3_enc->generate_master_secret(s,
                                                        s->
                                                        session->master_key,
                                                        pms, pmslen);
        OPENSSL_clear_free(pms, pmslen);
        s->s3->tmp.pms = NULL;
        if (s->session->master_key_length < 0) {
            ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_INTERNAL_ERROR);
            SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE, ERR_R_INTERNAL_ERROR);
            goto err;
        }
    }
    return n;
 memerr:
    ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_INTERNAL_ERROR);
    SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE, ERR_R_MALLOC_FAILURE);
 err:
    OPENSSL_clear_free(pms, pmslen);
    s->s3->tmp.pms = NULL;
#ifndef OPENSSL_NO_EC
    BN_CTX_free(bn_ctx);
    OPENSSL_free(encodedPoint);
    EC_KEY_free(clnt_ecdh);
    EVP_PKEY_free(srvr_pub_pkey);
#endif
    s->state = SSL_ST_ERR;
    return (-1);
}
