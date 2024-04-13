iakerb_tkt_creds_ctx(iakerb_ctx_id_t ctx,
                     krb5_gss_cred_id_t cred,
                     krb5_gss_name_t name,
                     OM_uint32 time_req)

{
    krb5_error_code code;
    krb5_creds creds;
    krb5_timestamp now;

    assert(cred->name != NULL);
    assert(cred->name->princ != NULL);

    memset(&creds, 0, sizeof(creds));

    creds.client = cred->name->princ;
    creds.server = name->princ;

    if (time_req != 0 && time_req != GSS_C_INDEFINITE) {
        code = krb5_timeofday(ctx->k5c, &now);
        if (code != 0)
            goto cleanup;

        creds.times.endtime = now + time_req;
    }

    if (cred->name->ad_context != NULL) {
        code = krb5_authdata_export_authdata(ctx->k5c,
                                             cred->name->ad_context,
                                             AD_USAGE_TGS_REQ,
                                             &creds.authdata);
        if (code != 0)
            goto cleanup;
    }

    code = krb5_tkt_creds_init(ctx->k5c, cred->ccache, &creds, 0, &ctx->tcc);
    if (code != 0)
        goto cleanup;

cleanup:
    krb5_free_authdata(ctx->k5c, creds.authdata);

    return code;
}
