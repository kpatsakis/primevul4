make_external_lucid_ctx_v1(
    krb5_gss_ctx_id_rec * gctx,
    int version,
    void **out_ptr)
{
    gss_krb5_lucid_context_v1_t *lctx = NULL;
    unsigned int bufsize = sizeof(gss_krb5_lucid_context_v1_t);
    krb5_error_code retval;

    /* Allocate the structure */
    if ((lctx = xmalloc(bufsize)) == NULL) {
        retval = ENOMEM;
        goto error_out;
    }

    memset(lctx, 0, bufsize);

    lctx->version = 1;
    lctx->initiate = gctx->initiate ? 1 : 0;
    lctx->endtime = gctx->krb_times.endtime;
    lctx->send_seq = gctx->seq_send;
    lctx->recv_seq = gctx->seq_recv;
    lctx->protocol = gctx->proto;
    /* gctx->proto == 0 ==> rfc1964-style key information
       gctx->proto == 1 ==> cfx-style (draft-ietf-krb-wg-gssapi-cfx-07) keys */
    if (gctx->proto == 0) {
        lctx->rfc1964_kd.sign_alg = gctx->signalg;
        lctx->rfc1964_kd.seal_alg = gctx->sealalg;
        /* Copy key */
        if ((retval = copy_keyblock_to_lucid_key(&gctx->seq->keyblock,
                                                 &lctx->rfc1964_kd.ctx_key)))
            goto error_out;
    }
    else if (gctx->proto == 1) {
        /* Copy keys */
        /* (subkey is always present, either a copy of the kerberos
           session key or a subkey) */
        if ((retval = copy_keyblock_to_lucid_key(&gctx->subkey->keyblock,
                                                 &lctx->cfx_kd.ctx_key)))
            goto error_out;
        if (gctx->have_acceptor_subkey) {
            if ((retval = copy_keyblock_to_lucid_key(&gctx->acceptor_subkey->keyblock,
                                                     &lctx->cfx_kd.acceptor_subkey)))
                goto error_out;
            lctx->cfx_kd.have_acceptor_subkey = 1;
        }
    }
    else {
        xfree(lctx);
        return EINVAL;  /* XXX better error code? */
    }

    /* Success! */
    *out_ptr = lctx;
    return 0;

error_out:
    if (lctx) {
        free_external_lucid_ctx_v1(lctx);
    }
    return retval;

}
