iakerb_gss_unwrap(OM_uint32 *minor_status, gss_ctx_id_t context_handle,
                  gss_buffer_t input_message_buffer,
                  gss_buffer_t output_message_buffer, int *conf_state,
                  gss_qop_t *qop_state)
{
    iakerb_ctx_id_t ctx = (iakerb_ctx_id_t)context_handle;

    if (ctx->gssc == GSS_C_NO_CONTEXT)
        return GSS_S_NO_CONTEXT;

    return krb5_gss_unwrap(minor_status, ctx->gssc, input_message_buffer,
                           output_message_buffer, conf_state, qop_state);
}
