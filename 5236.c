lsquic_qeh_write_avail (struct qpack_enc_hdl *qeh)
{
    if ((qeh->qeh_flags & QEH_INITIALIZED) && qeh->qeh_enc_sm_out)
        return lsquic_stream_write_avail(qeh->qeh_enc_sm_out);
    else if (qeh->qeh_flags & QEH_INITIALIZED)
        return ~((size_t) 0);   /* Unlimited write */
    else
        return 0;
}