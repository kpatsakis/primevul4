kg_get_ccache_name (OM_uint32 *minor_status, const char **out_name)
{
    const char *name = NULL;
    OM_uint32 err = 0;
    char *kg_ccache_name;

    kg_ccache_name = k5_getspecific(K5_KEY_GSS_KRB5_CCACHE_NAME);

    if (kg_ccache_name != NULL) {
        name = strdup(kg_ccache_name);
        if (name == NULL)
            err = ENOMEM;
    } else {
        krb5_context context = NULL;

        /* Reset the context default ccache (see text above), and then
           retrieve it.  */
        err = krb5_gss_init_context(&context);
        if (!err)
            err = krb5_cc_set_default_name (context, NULL);
        if (!err) {
            name = krb5_cc_default_name(context);
            if (name) {
                name = strdup(name);
                if (name == NULL)
                    err = ENOMEM;
            }
        }
        if (err && context)
            save_error_info(err, context);
        if (context)
            krb5_free_context(context);
    }

    if (!err) {
        if (out_name) {
            *out_name = name;
        }
    }

    *minor_status = err;
    return (*minor_status == 0) ? GSS_S_COMPLETE : GSS_S_FAILURE;
}
