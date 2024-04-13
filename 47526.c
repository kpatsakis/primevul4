iakerb_gss_pseudo_random(OM_uint32 *minor_status, gss_ctx_id_t context_handle,
                         int prf_key, const gss_buffer_t prf_in,
                         ssize_t desired_output_len, gss_buffer_t prf_out)
{
    iakerb_ctx_id_t ctx = (iakerb_ctx_id_t)context_handle;

    if (ctx->gssc == GSS_C_NO_CONTEXT)
        return GSS_S_NO_CONTEXT;

    return krb5_gss_pseudo_random(minor_status, ctx->gssc, prf_key, prf_in,
                                  desired_output_len, prf_out);
}
