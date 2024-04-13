iakerb_gss_wrap_size_limit(OM_uint32 *minor_status,
                           gss_ctx_id_t context_handle, int conf_req_flag,
                           gss_qop_t qop_req, OM_uint32 req_output_size,
                           OM_uint32 *max_input_size)
{
    iakerb_ctx_id_t ctx = (iakerb_ctx_id_t)context_handle;

    if (ctx->gssc == GSS_C_NO_CONTEXT)
        return GSS_S_NO_CONTEXT;

    return krb5_gss_wrap_size_limit(minor_status, ctx->gssc, conf_req_flag,
                                    qop_req, req_output_size, max_input_size);
}
