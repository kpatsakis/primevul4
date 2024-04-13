iakerb_gss_set_sec_context_option(OM_uint32 *minor_status,
                                  gss_ctx_id_t *context_handle,
                                  const gss_OID desired_object,
                                  const gss_buffer_t value)
{
    iakerb_ctx_id_t ctx = (iakerb_ctx_id_t)*context_handle;

    if (ctx == NULL || ctx->gssc == GSS_C_NO_CONTEXT)
        return GSS_S_UNAVAILABLE;

    return krb5_gss_set_sec_context_option(minor_status, &ctx->gssc,
                                           desired_object, value);
}
