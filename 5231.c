lsquic_qeh_max_prefix_size (const struct qpack_enc_hdl *qeh)
{
    if (qeh->qeh_flags & QEH_HAVE_SETTINGS)
        return qeh->qeh_max_prefix_size;
    else
        return LSQPACK_UINT64_ENC_SZ * 2;
}