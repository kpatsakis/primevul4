iakerb_gss_inquire_context(OM_uint32 *minor_status,
                           gss_ctx_id_t context_handle, gss_name_t *src_name,
                           gss_name_t *targ_name, OM_uint32 *lifetime_rec,
                           gss_OID *mech_type, OM_uint32 *ctx_flags,
                           int *initiate, int *opened)
{
    OM_uint32 ret;
    iakerb_ctx_id_t ctx = (iakerb_ctx_id_t)context_handle;

    if (src_name != NULL)
        *src_name = GSS_C_NO_NAME;
    if (targ_name != NULL)
        *targ_name = GSS_C_NO_NAME;
    if (lifetime_rec != NULL)
        *lifetime_rec = 0;
    if (mech_type != NULL)
        *mech_type = (gss_OID)gss_mech_iakerb;
    if (ctx_flags != NULL)
        *ctx_flags = 0;
    if (initiate != NULL)
        *initiate = ctx->initiate;
    if (opened != NULL)
        *opened = ctx->established;

    if (ctx->gssc == GSS_C_NO_CONTEXT)
        return GSS_S_COMPLETE;

    ret = krb5_gss_inquire_context(minor_status, ctx->gssc, src_name,
                                   targ_name, lifetime_rec, mech_type,
                                   ctx_flags, initiate, opened);

    if (!ctx->established) {
        /* Report IAKERB as the mech OID until the context is established. */
        if (mech_type != NULL)
            *mech_type = (gss_OID)gss_mech_iakerb;

        /* We don't support exporting partially-established contexts. */
        if (ctx_flags != NULL)
            *ctx_flags &= ~GSS_C_TRANS_FLAG;
    }

    return ret;
}
