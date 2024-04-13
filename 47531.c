iakerb_gss_wrap(OM_uint32 *minor_status, gss_ctx_id_t context_handle,
                int conf_req_flag, gss_qop_t qop_req,
                gss_buffer_t input_message_buffer, int *conf_state,
                gss_buffer_t output_message_buffer)
{
    iakerb_ctx_id_t ctx = (iakerb_ctx_id_t)context_handle;

    if (ctx->gssc == GSS_C_NO_CONTEXT)
        return GSS_S_NO_CONTEXT;

    return krb5_gss_wrap(minor_status, ctx->gssc, conf_req_flag, qop_req,
                         input_message_buffer, conf_state,
                         output_message_buffer);
}
