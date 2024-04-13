int gss_krb5int_lib_init(void)
{
    int err;

#ifdef SHOW_INITFINI_FUNCS
    printf("gss_krb5int_lib_init\n");
#endif

    add_error_table(&et_k5g_error_table);

#ifndef LEAN_CLIENT
    err = k5_mutex_finish_init(&gssint_krb5_keytab_lock);
    if (err)
        return err;
#endif /* LEAN_CLIENT */
    err = k5_key_register(K5_KEY_GSS_KRB5_SET_CCACHE_OLD_NAME, free);
    if (err)
        return err;
    err = k5_key_register(K5_KEY_GSS_KRB5_CCACHE_NAME, free);
    if (err)
        return err;
    err = k5_key_register(K5_KEY_GSS_KRB5_ERROR_MESSAGE,
                          krb5_gss_delete_error_info);
    if (err)
        return err;
#ifndef _WIN32
    err = k5_mutex_finish_init(&kg_kdc_flag_mutex);
    if (err)
        return err;
    err = k5_mutex_finish_init(&kg_vdb.mutex);
    if (err)
        return err;
#endif
#ifdef _GSS_STATIC_LINK
    err = gss_krb5mechglue_init();
    if (err)
        return err;
    err = gss_iakerbmechglue_init();
    if (err)
        return err;
#endif

    return 0;
}
