pkinit_server_get_flags(krb5_context kcontext, krb5_preauthtype patype)
{
    if (patype == KRB5_PADATA_PKINIT_KX)
        return PA_INFO;
    return PA_SUFFICIENT | PA_REPLACES_KEY | PA_TYPED_E_DATA;
}
