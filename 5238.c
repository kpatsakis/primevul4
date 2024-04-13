lsquic_qeh_cleanup (struct qpack_enc_hdl *qeh)
{
    if (qeh->qeh_flags & QEH_INITIALIZED)
    {
        LSQ_DEBUG("cleanup");
        if (qeh->qeh_exp_rec)
            qeh_log_and_clean_exp_rec(qeh);
        lsqpack_enc_cleanup(&qeh->qeh_encoder);
        lsquic_frab_list_cleanup(&qeh->qeh_fral);
        memset(qeh, 0, sizeof(*qeh));
    }
}