kdcpreauth_pkinit_initvt(krb5_context context, int maj_ver, int min_ver,
                         krb5_plugin_vtable vtable)
{
    krb5_kdcpreauth_vtable vt;

    if (maj_ver != 1)
        return KRB5_PLUGIN_VER_NOTSUPP;
    vt = (krb5_kdcpreauth_vtable)vtable;
    vt->name = "pkinit";
    vt->pa_type_list = supported_server_pa_types;
    vt->init = pkinit_server_plugin_init;
    vt->fini = pkinit_server_plugin_fini;
    vt->flags = pkinit_server_get_flags;
    vt->edata = pkinit_server_get_edata;
    vt->verify = pkinit_server_verify_padata;
    vt->return_padata = pkinit_server_return_padata;
    return 0;
}
