iakerb_gss_unwrap_iov(OM_uint32 *minor_status, gss_ctx_id_t context_handle,
                      int *conf_state, gss_qop_t *qop_state,
                      gss_iov_buffer_desc *iov, int iov_count)
{
    iakerb_ctx_id_t ctx = (iakerb_ctx_id_t)context_handle;

    if (ctx->gssc == GSS_C_NO_CONTEXT)
        return GSS_S_NO_CONTEXT;

    return krb5_gss_unwrap_iov(minor_status, ctx->gssc, conf_state, qop_state,
                               iov, iov_count);
}
