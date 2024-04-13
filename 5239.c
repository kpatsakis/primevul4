lsquic_qeh_enc_off (struct qpack_enc_hdl *qeh)
{
    if (qeh->qeh_enc_sm_out)
        return qeh->qeh_enc_sm_out->tosend_off;
    else
        return 0;
}