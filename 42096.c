load_cas_and_crls(krb5_context context,
                  pkinit_plg_crypto_context plg_cryptoctx,
                  pkinit_req_crypto_context req_cryptoctx,
                  pkinit_identity_crypto_context id_cryptoctx,
                  int catype,
                  char *filename)
{
    STACK_OF(X509_INFO) *sk = NULL;
    STACK_OF(X509) *ca_certs = NULL;
    STACK_OF(X509_CRL) *ca_crls = NULL;
    BIO *in = NULL;
    krb5_error_code retval = ENOMEM;
    int i = 0;

    /* If there isn't already a stack in the context,
     * create a temporary one now */
    switch(catype) {
    case CATYPE_ANCHORS:
        if (id_cryptoctx->trustedCAs != NULL)
            ca_certs = id_cryptoctx->trustedCAs;
        else {
            ca_certs = sk_X509_new_null();
            if (ca_certs == NULL)
                return ENOMEM;
        }
        break;
    case CATYPE_INTERMEDIATES:
        if (id_cryptoctx->intermediateCAs != NULL)
            ca_certs = id_cryptoctx->intermediateCAs;
        else {
            ca_certs = sk_X509_new_null();
            if (ca_certs == NULL)
                return ENOMEM;
        }
        break;
    case CATYPE_CRLS:
        if (id_cryptoctx->revoked != NULL)
            ca_crls = id_cryptoctx->revoked;
        else {
            ca_crls = sk_X509_CRL_new_null();
            if (ca_crls == NULL)
                return ENOMEM;
        }
        break;
    default:
        return ENOTSUP;
    }

    if (!(in = BIO_new_file(filename, "r"))) {
        retval = errno;
        pkiDebug("%s: error opening file '%s': %s\n", __FUNCTION__,
                 filename, error_message(errno));
        goto cleanup;
    }

    /* This loads from a file, a stack of x509/crl/pkey sets */
    if ((sk = PEM_X509_INFO_read_bio(in, NULL, NULL, NULL)) == NULL) {
        pkiDebug("%s: error reading file '%s'\n", __FUNCTION__, filename);
        retval = EIO;
        goto cleanup;
    }

    /* scan over the stack created from loading the file contents,
     * weed out duplicates, and push new ones onto the return stack
     */
    for (i = 0; i < sk_X509_INFO_num(sk); i++) {
        X509_INFO *xi = sk_X509_INFO_value(sk, i);
        if (xi != NULL && xi->x509 != NULL && catype != CATYPE_CRLS) {
            int j = 0, size = sk_X509_num(ca_certs), flag = 0;

            if (!size) {
                sk_X509_push(ca_certs, xi->x509);
                xi->x509 = NULL;
                continue;
            }
            for (j = 0; j < size; j++) {
                X509 *x = sk_X509_value(ca_certs, j);
                flag = X509_cmp(x, xi->x509);
                if (flag == 0)
                    break;
                else
                    continue;
            }
            if (flag != 0) {
                sk_X509_push(ca_certs, X509_dup(xi->x509));
            }
        } else if (xi != NULL && xi->crl != NULL && catype == CATYPE_CRLS) {
            int j = 0, size = sk_X509_CRL_num(ca_crls), flag = 0;
            if (!size) {
                sk_X509_CRL_push(ca_crls, xi->crl);
                xi->crl = NULL;
                continue;
            }
            for (j = 0; j < size; j++) {
                X509_CRL *x = sk_X509_CRL_value(ca_crls, j);
                flag = X509_CRL_cmp(x, xi->crl);
                if (flag == 0)
                    break;
                else
                    continue;
            }
            if (flag != 0) {
                sk_X509_CRL_push(ca_crls, X509_CRL_dup(xi->crl));
            }
        }
    }

    /* If we added something and there wasn't a stack in the
     * context before, add the temporary stack to the context.
     */
    switch(catype) {
    case CATYPE_ANCHORS:
        if (sk_X509_num(ca_certs) == 0) {
            pkiDebug("no anchors in file, %s\n", filename);
            if (id_cryptoctx->trustedCAs == NULL)
                sk_X509_free(ca_certs);
        } else {
            if (id_cryptoctx->trustedCAs == NULL)
                id_cryptoctx->trustedCAs = ca_certs;
        }
        break;
    case CATYPE_INTERMEDIATES:
        if (sk_X509_num(ca_certs) == 0) {
            pkiDebug("no intermediates in file, %s\n", filename);
            if (id_cryptoctx->intermediateCAs == NULL)
                sk_X509_free(ca_certs);
        } else {
            if (id_cryptoctx->intermediateCAs == NULL)
                id_cryptoctx->intermediateCAs = ca_certs;
        }
        break;
    case CATYPE_CRLS:
        if (sk_X509_CRL_num(ca_crls) == 0) {
            pkiDebug("no crls in file, %s\n", filename);
            if (id_cryptoctx->revoked == NULL)
                sk_X509_CRL_free(ca_crls);
        } else {
            if (id_cryptoctx->revoked == NULL)
                id_cryptoctx->revoked = ca_crls;
        }
        break;
    default:
        /* Should have been caught above! */
        retval = EINVAL;
        goto cleanup;
        break;
    }

    retval = 0;

cleanup:
    if (in != NULL)
        BIO_free(in);
    if (sk != NULL)
        sk_X509_INFO_pop_free(sk, X509_INFO_free);

    return retval;
}
