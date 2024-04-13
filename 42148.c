server_process_dh(krb5_context context,
                  pkinit_plg_crypto_context plg_cryptoctx,
                  pkinit_req_crypto_context cryptoctx,
                  pkinit_identity_crypto_context id_cryptoctx,
                  unsigned char *data,
                  unsigned int data_len,
                  unsigned char **dh_pubkey,
                  unsigned int *dh_pubkey_len,
                  unsigned char **server_key,
                  unsigned int *server_key_len)
{
    krb5_error_code retval = ENOMEM;
    DH *dh = NULL, *dh_server = NULL;
    unsigned char *p = NULL;
    ASN1_INTEGER *pub_key = NULL;

    *dh_pubkey = *server_key = NULL;
    *dh_pubkey_len = *server_key_len = 0;

    /* get client's received DH parameters that we saved in server_check_dh */
    dh = cryptoctx->dh;

    dh_server = DH_new();
    if (dh_server == NULL)
        goto cleanup;
    dh_server->p = BN_dup(dh->p);
    dh_server->g = BN_dup(dh->g);
    dh_server->q = BN_dup(dh->q);

    /* decode client's public key */
    p = data;
    pub_key = d2i_ASN1_INTEGER(NULL, (const unsigned char **)&p, (int)data_len);
    if (pub_key == NULL)
        goto cleanup;
    dh->pub_key = ASN1_INTEGER_to_BN(pub_key, NULL);
    if (dh->pub_key == NULL)
        goto cleanup;
    ASN1_INTEGER_free(pub_key);

    if (!DH_generate_key(dh_server))
        goto cleanup;

    /* generate DH session key */
    *server_key_len = DH_size(dh_server);
    if ((*server_key = malloc(*server_key_len)) == NULL)
        goto cleanup;
    compute_dh(*server_key, *server_key_len, dh->pub_key, dh_server);

#ifdef DEBUG_DH
    print_dh(dh_server, "client&server's DH params\n");
    print_pubkey(dh->pub_key, "client's pub_key=");
    print_pubkey(dh_server->pub_key, "server's pub_key=");
    pkiDebug("server computed key=");
    print_buffer(*server_key, *server_key_len);
#endif

    /* KDC reply */
    /* pack DH public key */
    /* Diffie-Hellman public key must be ASN1 encoded as an INTEGER; this
     * encoding shall be used as the contents (the value) of the
     * subjectPublicKey component (a BIT STRING) of the SubjectPublicKeyInfo
     * data element
     */
    if ((pub_key = BN_to_ASN1_INTEGER(dh_server->pub_key, NULL)) == NULL)
        goto cleanup;
    *dh_pubkey_len = i2d_ASN1_INTEGER(pub_key, NULL);
    if ((p = *dh_pubkey = malloc(*dh_pubkey_len)) == NULL)
        goto cleanup;
    i2d_ASN1_INTEGER(pub_key, &p);
    if (pub_key != NULL)
        ASN1_INTEGER_free(pub_key);

    retval = 0;

    if (dh_server != NULL)
        DH_free(dh_server);
    return retval;

cleanup:
    if (dh_server != NULL)
        DH_free(dh_server);
    free(*dh_pubkey);
    free(*server_key);

    return retval;
}
