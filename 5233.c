lsquic_qeh_init (struct qpack_enc_hdl *qeh, struct lsquic_conn *conn)
{
    assert(!(qeh->qeh_flags & QEH_INITIALIZED));
    qeh->qeh_conn = conn;
    lsquic_frab_list_init(&qeh->qeh_fral, 0x400, NULL, NULL, NULL);
    lsqpack_enc_preinit(&qeh->qeh_encoder, (void *) conn);
    qeh->qeh_flags |= QEH_INITIALIZED;
    qeh->qeh_max_prefix_size =
                        lsqpack_enc_header_block_prefix_size(&qeh->qeh_encoder);
    if (qeh->qeh_dec_sm_in)
        lsquic_stream_wantread(qeh->qeh_dec_sm_in, 1);
    LSQ_DEBUG("initialized");
}