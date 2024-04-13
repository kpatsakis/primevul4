iakerb_gss_verify_mic(OM_uint32 *minor_status, gss_ctx_id_t context_handle,
                      gss_buffer_t msg_buffer, gss_buffer_t token_buffer,
                      gss_qop_t *qop_state)
{
    iakerb_ctx_id_t ctx = (iakerb_ctx_id_t)context_handle;

    if (ctx->gssc == GSS_C_NO_CONTEXT)
        return GSS_S_NO_CONTEXT;

    return krb5_gss_verify_mic(minor_status, ctx->gssc, msg_buffer,
                               token_buffer, qop_state);
}
