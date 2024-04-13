kg_caller_provided_ccache_name (OM_uint32 *minor_status,
                                int *out_caller_provided_name)
{
    if (out_caller_provided_name) {
        *out_caller_provided_name =
            (k5_getspecific(K5_KEY_GSS_KRB5_CCACHE_NAME) != NULL);
    }

    *minor_status = 0;
    return GSS_S_COMPLETE;
}
