void gss_krb5int_lib_fini(void)
{
#ifndef _GSS_STATIC_LINK
    if (!INITIALIZER_RAN(gss_krb5int_lib_init) || PROGRAM_EXITING()) {
# ifdef SHOW_INITFINI_FUNCS
        printf("gss_krb5int_lib_fini: skipping\n");
# endif
        return;
    }
#endif
#ifdef SHOW_INITFINI_FUNCS
    printf("gss_krb5int_lib_fini\n");
#endif
    remove_error_table(&et_k5g_error_table);

    k5_key_delete(K5_KEY_GSS_KRB5_SET_CCACHE_OLD_NAME);
    k5_key_delete(K5_KEY_GSS_KRB5_CCACHE_NAME);
    k5_key_delete(K5_KEY_GSS_KRB5_ERROR_MESSAGE);
    k5_mutex_destroy(&kg_vdb.mutex);
#ifndef _WIN32
    k5_mutex_destroy(&kg_kdc_flag_mutex);
#endif
#ifndef LEAN_CLIENT
    k5_mutex_destroy(&gssint_krb5_keytab_lock);
#endif /* LEAN_CLIENT */
}
