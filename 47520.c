iakerb_gss_get_mic(OM_uint32 *minor_status, gss_ctx_id_t context_handle,
                   gss_qop_t qop_req, gss_buffer_t message_buffer,
                   gss_buffer_t message_token)
{
    iakerb_ctx_id_t ctx = (iakerb_ctx_id_t)context_handle;

    if (ctx->gssc == GSS_C_NO_CONTEXT)
        return GSS_S_NO_CONTEXT;

    return krb5_gss_get_mic(minor_status, ctx->gssc, qop_req, message_buffer,
                            message_token);
}
