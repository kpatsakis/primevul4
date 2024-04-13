iakerb_init_creds_ctx(iakerb_ctx_id_t ctx,
                      krb5_gss_cred_id_t cred,
                      OM_uint32 time_req)
{
    krb5_error_code code;

    if (cred->iakerb_mech == 0) {
        code = EINVAL;
        goto cleanup;
    }

    assert(cred->name != NULL);
    assert(cred->name->princ != NULL);

    code = krb5_get_init_creds_opt_alloc(ctx->k5c, &ctx->gic_opts);
    if (code != 0)
        goto cleanup;

    if (time_req != 0 && time_req != GSS_C_INDEFINITE)
        krb5_get_init_creds_opt_set_tkt_life(ctx->gic_opts, time_req);

    code = krb5_get_init_creds_opt_set_out_ccache(ctx->k5c, ctx->gic_opts,
                                                  cred->ccache);
    if (code != 0)
        goto cleanup;

    code = krb5_init_creds_init(ctx->k5c,
                                cred->name->princ,
                                NULL,   /* prompter */
                                NULL,   /* data */
                                0,      /* start_time */
                                ctx->gic_opts,
                                &ctx->icc);
    if (code != 0)
        goto cleanup;

    if (cred->password != NULL) {
        code = krb5_init_creds_set_password(ctx->k5c, ctx->icc,
                                            cred->password);
    } else {
        code = krb5_init_creds_set_keytab(ctx->k5c, ctx->icc,
                                          cred->client_keytab);
    }
    if (code != 0)
        goto cleanup;

cleanup:
    return code;
}
