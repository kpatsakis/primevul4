iakerb_gss_context_time(OM_uint32 *minor_status, gss_ctx_id_t context_handle,
                        OM_uint32 *time_rec)
{
    iakerb_ctx_id_t ctx = (iakerb_ctx_id_t)context_handle;

    if (ctx->gssc == GSS_C_NO_CONTEXT)
        return GSS_S_NO_CONTEXT;

    return krb5_gss_context_time(minor_status, ctx->gssc, time_rec);
}
