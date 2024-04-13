qeh_begin_out (struct qpack_enc_hdl *qeh)
{
    if (0 == qeh_write_type(qeh)
        && (qeh->qeh_tsu_sz == 0
            || 0 == lsquic_frab_list_write(&qeh->qeh_fral, qeh->qeh_tsu_buf,
                                                            qeh->qeh_tsu_sz)))
    {
        LSQ_DEBUG("wrote %zu bytes to frab list", 1 + qeh->qeh_tsu_sz);
        lsquic_stream_wantwrite(qeh->qeh_enc_sm_out, 1);
    }
    else
    {
        LSQ_WARN("could not write to frab list");
        qeh->qeh_conn->cn_if->ci_internal_error(qeh->qeh_conn,
                                            "cannot write to frab list");
    }
}