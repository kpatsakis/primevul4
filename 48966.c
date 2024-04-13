gss_krb5int_extract_authtime_from_sec_context(OM_uint32 *minor_status,
                                              const gss_ctx_id_t context_handle,
                                              const gss_OID desired_oid,
                                              gss_buffer_set_t *data_set)
{
    krb5_gss_ctx_id_rec *ctx;
    gss_buffer_desc rep;

    ctx = (krb5_gss_ctx_id_rec *) context_handle;

    rep.value = &ctx->krb_times.authtime;
    rep.length = sizeof(ctx->krb_times.authtime);

    return generic_gss_add_buffer_set_member(minor_status, &rep, data_set);
}
