pkinit_get_certs_pkcs11(krb5_context context,
                        pkinit_plg_crypto_context plg_cryptoctx,
                        pkinit_req_crypto_context req_cryptoctx,
                        pkinit_identity_opts *idopts,
                        pkinit_identity_crypto_context id_cryptoctx,
                        krb5_principal princ)
{
#ifdef PKINIT_USE_MECH_LIST
    CK_MECHANISM_TYPE_PTR mechp;
    CK_MECHANISM_INFO info;
#endif
    CK_OBJECT_CLASS cls;
    CK_OBJECT_HANDLE obj;
    CK_ATTRIBUTE attrs[4];
    CK_ULONG count;
    CK_CERTIFICATE_TYPE certtype;
    CK_BYTE_PTR cert = NULL, cert_id;
    const unsigned char *cp;
    int i, r;
    unsigned int nattrs;
    X509 *x = NULL;

    /* Copy stuff from idopts -> id_cryptoctx */
    if (idopts->p11_module_name != NULL) {
        id_cryptoctx->p11_module_name = strdup(idopts->p11_module_name);
        if (id_cryptoctx->p11_module_name == NULL)
            return ENOMEM;
    }
    if (idopts->token_label != NULL) {
        id_cryptoctx->token_label = strdup(idopts->token_label);
        if (id_cryptoctx->token_label == NULL)
            return ENOMEM;
    }
    if (idopts->cert_label != NULL) {
        id_cryptoctx->cert_label = strdup(idopts->cert_label);
        if (id_cryptoctx->cert_label == NULL)
            return ENOMEM;
    }
    /* Convert the ascii cert_id string into a binary blob */
    if (idopts->cert_id_string != NULL) {
        BIGNUM *bn = NULL;
        BN_hex2bn(&bn, idopts->cert_id_string);
        if (bn == NULL)
            return ENOMEM;
        id_cryptoctx->cert_id_len = BN_num_bytes(bn);
        id_cryptoctx->cert_id = malloc((size_t) id_cryptoctx->cert_id_len);
        if (id_cryptoctx->cert_id == NULL) {
            BN_free(bn);
            return ENOMEM;
        }
        BN_bn2bin(bn, id_cryptoctx->cert_id);
        BN_free(bn);
    }
    id_cryptoctx->slotid = idopts->slotid;
    id_cryptoctx->pkcs11_method = 1;

    if (pkinit_open_session(context, id_cryptoctx)) {
        pkiDebug("can't open pkcs11 session\n");
        return KRB5KDC_ERR_PREAUTH_FAILED;
    }

#ifndef PKINIT_USE_MECH_LIST
    /*
     * We'd like to use CKM_SHA1_RSA_PKCS for signing if it's available, but
     * many cards seems to be confused about whether they are capable of
     * this or not. The safe thing seems to be to ignore the mechanism list,
     * always use CKM_RSA_PKCS and calculate the sha1 digest ourselves.
     */

    id_cryptoctx->mech = CKM_RSA_PKCS;
#else
    if ((r = id_cryptoctx->p11->C_GetMechanismList(id_cryptoctx->slotid, NULL,
                                                   &count)) != CKR_OK || count <= 0) {
        pkiDebug("C_GetMechanismList: %s\n", pkinit_pkcs11_code_to_text(r));
        return KRB5KDC_ERR_PREAUTH_FAILED;
    }
    mechp = malloc(count * sizeof (CK_MECHANISM_TYPE));
    if (mechp == NULL)
        return ENOMEM;
    if ((r = id_cryptoctx->p11->C_GetMechanismList(id_cryptoctx->slotid,
                                                   mechp, &count)) != CKR_OK)
        return KRB5KDC_ERR_PREAUTH_FAILED;
    for (i = 0; i < count; i++) {
        if ((r = id_cryptoctx->p11->C_GetMechanismInfo(id_cryptoctx->slotid,
                                                       mechp[i], &info)) != CKR_OK)
            return KRB5KDC_ERR_PREAUTH_FAILED;
#ifdef DEBUG_MECHINFO
        pkiDebug("mech %x flags %x\n", (int) mechp[i], (int) info.flags);
        if ((info.flags & (CKF_SIGN|CKF_DECRYPT)) == (CKF_SIGN|CKF_DECRYPT))
            pkiDebug("  this mech is good for sign & decrypt\n");
#endif
        if (mechp[i] == CKM_RSA_PKCS) {
            /* This seems backwards... */
            id_cryptoctx->mech =
                (info.flags & CKF_SIGN) ? CKM_SHA1_RSA_PKCS : CKM_RSA_PKCS;
        }
    }
    free(mechp);

    pkiDebug("got %d mechs from card\n", (int) count);
#endif

    cls = CKO_CERTIFICATE;
    attrs[0].type = CKA_CLASS;
    attrs[0].pValue = &cls;
    attrs[0].ulValueLen = sizeof cls;

    certtype = CKC_X_509;
    attrs[1].type = CKA_CERTIFICATE_TYPE;
    attrs[1].pValue = &certtype;
    attrs[1].ulValueLen = sizeof certtype;

    nattrs = 2;

    /* If a cert id and/or label were given, use them too */
    if (id_cryptoctx->cert_id_len > 0) {
        attrs[nattrs].type = CKA_ID;
        attrs[nattrs].pValue = id_cryptoctx->cert_id;
        attrs[nattrs].ulValueLen = id_cryptoctx->cert_id_len;
        nattrs++;
    }
    if (id_cryptoctx->cert_label != NULL) {
        attrs[nattrs].type = CKA_LABEL;
        attrs[nattrs].pValue = id_cryptoctx->cert_label;
        attrs[nattrs].ulValueLen = strlen(id_cryptoctx->cert_label);
        nattrs++;
    }

    r = id_cryptoctx->p11->C_FindObjectsInit(id_cryptoctx->session, attrs, nattrs);
    if (r != CKR_OK) {
        pkiDebug("C_FindObjectsInit: %s\n", pkinit_pkcs11_code_to_text(r));
        return KRB5KDC_ERR_PREAUTH_FAILED;
    }

    for (i = 0; ; i++) {
        if (i >= MAX_CREDS_ALLOWED)
            return KRB5KDC_ERR_PREAUTH_FAILED;

        /* Look for x.509 cert */
        if ((r = id_cryptoctx->p11->C_FindObjects(id_cryptoctx->session,
                                                  &obj, 1, &count)) != CKR_OK || count <= 0) {
            id_cryptoctx->creds[i] = NULL;
            break;
        }

        /* Get cert and id len */
        attrs[0].type = CKA_VALUE;
        attrs[0].pValue = NULL;
        attrs[0].ulValueLen = 0;

        attrs[1].type = CKA_ID;
        attrs[1].pValue = NULL;
        attrs[1].ulValueLen = 0;

        if ((r = id_cryptoctx->p11->C_GetAttributeValue(id_cryptoctx->session,
                                                        obj, attrs, 2)) != CKR_OK && r != CKR_BUFFER_TOO_SMALL) {
            pkiDebug("C_GetAttributeValue: %s\n", pkinit_pkcs11_code_to_text(r));
            return KRB5KDC_ERR_PREAUTH_FAILED;
        }
        cert = (CK_BYTE_PTR) malloc((size_t) attrs[0].ulValueLen + 1);
        cert_id = (CK_BYTE_PTR) malloc((size_t) attrs[1].ulValueLen + 1);
        if (cert == NULL || cert_id == NULL)
            return ENOMEM;

        /* Read the cert and id off the card */

        attrs[0].type = CKA_VALUE;
        attrs[0].pValue = cert;

        attrs[1].type = CKA_ID;
        attrs[1].pValue = cert_id;

        if ((r = id_cryptoctx->p11->C_GetAttributeValue(id_cryptoctx->session,
                                                        obj, attrs, 2)) != CKR_OK) {
            pkiDebug("C_GetAttributeValue: %s\n", pkinit_pkcs11_code_to_text(r));
            return KRB5KDC_ERR_PREAUTH_FAILED;
        }

        pkiDebug("cert %d size %d id %d idlen %d\n", i,
                 (int) attrs[0].ulValueLen, (int) cert_id[0],
                 (int) attrs[1].ulValueLen);

        cp = (unsigned char *) cert;
        x = d2i_X509(NULL, &cp, (int) attrs[0].ulValueLen);
        if (x == NULL)
            return KRB5KDC_ERR_PREAUTH_FAILED;
        id_cryptoctx->creds[i] = malloc(sizeof(struct _pkinit_cred_info));
        if (id_cryptoctx->creds[i] == NULL)
            return KRB5KDC_ERR_PREAUTH_FAILED;
        id_cryptoctx->creds[i]->name = reassemble_pkcs11_name(idopts);
        id_cryptoctx->creds[i]->cert = x;
        id_cryptoctx->creds[i]->key = NULL;
        id_cryptoctx->creds[i]->cert_id = cert_id;
        id_cryptoctx->creds[i]->cert_id_len = attrs[1].ulValueLen;
        free(cert);
    }
    id_cryptoctx->p11->C_FindObjectsFinal(id_cryptoctx->session);
    if (cert == NULL)
        return KRB5KDC_ERR_PREAUTH_FAILED;
    return 0;
}
