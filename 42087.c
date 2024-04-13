crypto_load_certs(krb5_context context,
                  pkinit_plg_crypto_context plg_cryptoctx,
                  pkinit_req_crypto_context req_cryptoctx,
                  pkinit_identity_opts *idopts,
                  pkinit_identity_crypto_context id_cryptoctx,
                  krb5_principal princ)
{
    krb5_error_code retval;

    switch(idopts->idtype) {
    case IDTYPE_FILE:
        retval = pkinit_get_certs_fs(context, plg_cryptoctx,
                                     req_cryptoctx, idopts,
                                     id_cryptoctx, princ);
        break;
    case IDTYPE_DIR:
        retval = pkinit_get_certs_dir(context, plg_cryptoctx,
                                      req_cryptoctx, idopts,
                                      id_cryptoctx, princ);
        break;
#ifndef WITHOUT_PKCS11
    case IDTYPE_PKCS11:
        retval = pkinit_get_certs_pkcs11(context, plg_cryptoctx,
                                         req_cryptoctx, idopts,
                                         id_cryptoctx, princ);
        break;
#endif
    case IDTYPE_PKCS12:
        retval = pkinit_get_certs_pkcs12(context, plg_cryptoctx,
                                         req_cryptoctx, idopts,
                                         id_cryptoctx, princ);
        break;
    default:
        retval = EINVAL;
    }
    if (retval)
        goto cleanup;

cleanup:
    return retval;
}
