crypto_cert_get_matching_data(krb5_context context,
                              pkinit_cert_handle ch,
                              pkinit_cert_matching_data **ret_md)
{
    krb5_error_code retval;
    pkinit_cert_matching_data *md;
    krb5_principal *pkinit_sans =NULL, *upn_sans = NULL;
    struct _pkinit_cert_data *cd = (struct _pkinit_cert_data *)ch;
    unsigned int i, j;
    char buf[DN_BUF_LEN];
    unsigned int bufsize = sizeof(buf);

    if (cd == NULL || cd->magic != CERT_MAGIC)
        return EINVAL;
    if (ret_md == NULL)
        return EINVAL;

    md = calloc(1, sizeof(*md));
    if (md == NULL)
        return ENOMEM;

    md->ch = ch;

    /* get the subject name (in rfc2253 format) */
    X509_NAME_oneline_ex(X509_get_subject_name(cd->cred->cert),
                         buf, &bufsize, XN_FLAG_SEP_COMMA_PLUS);
    md->subject_dn = strdup(buf);
    if (md->subject_dn == NULL) {
        retval = ENOMEM;
        goto cleanup;
    }

    /* get the issuer name (in rfc2253 format) */
    X509_NAME_oneline_ex(X509_get_issuer_name(cd->cred->cert),
                         buf, &bufsize, XN_FLAG_SEP_COMMA_PLUS);
    md->issuer_dn = strdup(buf);
    if (md->issuer_dn == NULL) {
        retval = ENOMEM;
        goto cleanup;
    }

    /* get the san data */
    retval = crypto_retrieve_X509_sans(context, cd->plgctx, cd->reqctx,
                                       cd->cred->cert, &pkinit_sans,
                                       &upn_sans, NULL);
    if (retval)
        goto cleanup;

    j = 0;
    if (pkinit_sans != NULL) {
        for (i = 0; pkinit_sans[i] != NULL; i++)
            j++;
    }
    if (upn_sans != NULL) {
        for (i = 0; upn_sans[i] != NULL; i++)
            j++;
    }
    if (j != 0) {
        md->sans = calloc((size_t)j+1, sizeof(*md->sans));
        if (md->sans == NULL) {
            retval = ENOMEM;
            goto cleanup;
        }
        j = 0;
        if (pkinit_sans != NULL) {
            for (i = 0; pkinit_sans[i] != NULL; i++)
                md->sans[j++] = pkinit_sans[i];
            free(pkinit_sans);
        }
        if (upn_sans != NULL) {
            for (i = 0; upn_sans[i] != NULL; i++)
                md->sans[j++] = upn_sans[i];
            free(upn_sans);
        }
        md->sans[j] = NULL;
    } else
        md->sans = NULL;

    /* get the KU and EKU data */

    retval = crypto_retieve_X509_key_usage(context, cd->plgctx, cd->reqctx,
                                           cd->cred->cert,
                                           &md->ku_bits, &md->eku_bits);
    if (retval)
        goto cleanup;

    *ret_md = md;
    retval = 0;
cleanup:
    if (retval) {
        if (md)
            crypto_cert_free_matching_data(context, md);
    }
    return retval;
}
