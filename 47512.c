krb5_gss_set_sec_context_option (OM_uint32 *minor_status,
                                 gss_ctx_id_t *context_handle,
                                 const gss_OID desired_object,
                                 const gss_buffer_t value)
{
#if 0
    size_t i;
#endif

    if (minor_status == NULL)
        return GSS_S_CALL_INACCESSIBLE_WRITE;

    *minor_status = 0;

    if (context_handle == NULL)
        return GSS_S_CALL_INACCESSIBLE_READ;

    if (desired_object == GSS_C_NO_OID)
        return GSS_S_CALL_INACCESSIBLE_READ;

#if 0
    for (i = 0; i < sizeof(krb5_gss_set_sec_context_option_ops)/
             sizeof(krb5_gss_set_sec_context_option_ops[0]); i++) {
        if (g_OID_prefix_equal(desired_object, &krb5_gss_set_sec_context_option_ops[i].oid)) {
            return (*krb5_gss_set_sec_context_option_ops[i].func)(minor_status,
                                                                  context_handle,
                                                                  desired_object,
                                                                  value);
        }
    }
#endif

    *minor_status = EINVAL;

    return GSS_S_UNAVAILABLE;
}
