iakerb_gss_process_context_token(OM_uint32 *minor_status,
                                 const gss_ctx_id_t context_handle,
                                 const gss_buffer_t token_buffer)
{
    iakerb_ctx_id_t ctx = (iakerb_ctx_id_t)context_handle;

    if (ctx->gssc == GSS_C_NO_CONTEXT)
        return GSS_S_DEFECTIVE_TOKEN;

    return krb5_gss_process_context_token(minor_status, ctx->gssc,
                                          token_buffer);
}
