client_process_dh(krb5_context context,
                  pkinit_plg_crypto_context plg_cryptoctx,
                  pkinit_req_crypto_context cryptoctx,
                  pkinit_identity_crypto_context id_cryptoctx,
                  unsigned char *subjectPublicKey_data,
                  unsigned int subjectPublicKey_length,
                  unsigned char **client_key,
                  unsigned int *client_key_len)
{
    krb5_error_code retval = KRB5KDC_ERR_PREAUTH_FAILED;
    BIGNUM *server_pub_key = NULL;
    ASN1_INTEGER *pub_key = NULL;
    const unsigned char *p = NULL;

    *client_key_len = DH_size(cryptoctx->dh);
    if ((*client_key = malloc(*client_key_len)) == NULL) {
        retval = ENOMEM;
        goto cleanup;
    }
    p = subjectPublicKey_data;
    pub_key = d2i_ASN1_INTEGER(NULL, &p, (long)subjectPublicKey_length);
    if (pub_key == NULL)
        goto cleanup;
    if ((server_pub_key = ASN1_INTEGER_to_BN(pub_key, NULL)) == NULL)
        goto cleanup;

    compute_dh(*client_key, *client_key_len, server_pub_key, cryptoctx->dh);
#ifdef DEBUG_DH
    print_pubkey(server_pub_key, "server's pub_key=");
    pkiDebug("client computed key (%d)= ", *client_key_len);
    print_buffer(*client_key, *client_key_len);
#endif

    retval = 0;
    if (server_pub_key != NULL)
        BN_free(server_pub_key);
    if (pub_key != NULL)
        ASN1_INTEGER_free(pub_key);

    return retval;

cleanup:
    free(*client_key);
    *client_key = NULL;
    if (pub_key != NULL)
        ASN1_INTEGER_free(pub_key);

    return retval;
}
