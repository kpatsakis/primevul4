reassemble_pkcs11_name(pkinit_identity_opts *idopts)
{
    struct k5buf buf;
    int n = 0;
    char *ret;

    krb5int_buf_init_dynamic(&buf);
    krb5int_buf_add(&buf, "PKCS11:");
    n = 0;
    if (idopts->p11_module_name != NULL) {
        krb5int_buf_add_fmt(&buf, "%smodule_name=%s",
                            n++ ? "," : "",
                            idopts->p11_module_name);
    }
    if (idopts->token_label != NULL) {
        krb5int_buf_add_fmt(&buf, "%stoken=%s",
                            n++ ? "," : "",
                            idopts->token_label);
    }
    if (idopts->cert_label != NULL) {
        krb5int_buf_add_fmt(&buf, "%scertlabel=%s",
                            n++ ? "," : "",
                            idopts->cert_label);
    }
    if (idopts->cert_id_string != NULL) {
        krb5int_buf_add_fmt(&buf, "%scertid=%s",
                            n++ ? "," : "",
                            idopts->cert_id_string);
    }
    if (idopts->slotid != PK_NOSLOT) {
        krb5int_buf_add_fmt(&buf, "%sslotid=%ld",
                            n++ ? "," : "",
                            (long)idopts->slotid);
    }
    if (krb5int_buf_len(&buf) >= 0)
        ret = strdup(krb5int_buf_data(&buf));
    else
        ret = NULL;
    krb5int_free_buf(&buf);
    return ret;
}
