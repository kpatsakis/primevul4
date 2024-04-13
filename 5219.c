qeh_log_and_clean_exp_rec (struct qpack_enc_hdl *qeh)
{
    char buf[0x400];

    qeh->qeh_exp_rec->qer_comp_ratio = lsqpack_enc_ratio(&qeh->qeh_encoder);
    (void) lsquic_qpack_exp_to_xml(qeh->qeh_exp_rec, buf, sizeof(buf));
    LSQ_NOTICE("%s", buf);
    lsquic_qpack_exp_destroy(qeh->qeh_exp_rec);
    qeh->qeh_exp_rec = NULL;
}