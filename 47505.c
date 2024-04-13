kg_sync_ccache_name (krb5_context context, OM_uint32 *minor_status)
{
    OM_uint32 err = 0;

    /*
     * Sync up the context ccache name with the GSSAPI ccache name.
     * If kg_ccache_name is NULL -- normal unless someone has called
     * gss_krb5_ccache_name() -- then the system default ccache will
     * be picked up and used by resetting the context default ccache.
     * This is needed for platforms which support multiple ccaches.
     */

    if (!err) {
        /* if NULL, resets the context default ccache */
        err = krb5_cc_set_default_name(context,
                                       (char *) k5_getspecific(K5_KEY_GSS_KRB5_CCACHE_NAME));
    }

    *minor_status = err;
    return (*minor_status == 0) ? GSS_S_COMPLETE : GSS_S_FAILURE;
}
