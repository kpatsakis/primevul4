crypto_load_cas_and_crls(krb5_context context,
                         pkinit_plg_crypto_context plg_cryptoctx,
                         pkinit_req_crypto_context req_cryptoctx,
                         pkinit_identity_opts *idopts,
                         pkinit_identity_crypto_context id_cryptoctx,
                         int idtype,
                         int catype,
                         char *id)
{
    pkiDebug("%s: called with idtype %s and catype %s\n",
             __FUNCTION__, idtype2string(idtype), catype2string(catype));
    switch (idtype) {
    case IDTYPE_FILE:
        return load_cas_and_crls(context, plg_cryptoctx, req_cryptoctx,
                                 id_cryptoctx, catype, id);
        break;
    case IDTYPE_DIR:
        return load_cas_and_crls_dir(context, plg_cryptoctx, req_cryptoctx,
                                     id_cryptoctx, catype, id);
        break;
    default:
        return ENOTSUP;
        break;
    }
}
