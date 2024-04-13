qeh_write_type (struct qpack_enc_hdl *qeh)
{
    int s;

#ifndef NDEBUG
    const char *env = getenv("LSQUIC_RND_VARINT_LEN");
    if (env && atoi(env))
    {
        s = rand() & 3;
        LSQ_DEBUG("writing %d-byte stream type", 1 << s);
    }
    else
#endif
        s = 0;

    switch (s)
    {
    case 0:
        return lsquic_frab_list_write(&qeh->qeh_fral,
                                (unsigned char []) { HQUST_QPACK_ENC }, 1);
    case 1:
        return lsquic_frab_list_write(&qeh->qeh_fral,
                            (unsigned char []) { 0x40, HQUST_QPACK_ENC }, 2);
    case 2:
        return lsquic_frab_list_write(&qeh->qeh_fral,
                (unsigned char []) { 0x80, 0x00, 0x00, HQUST_QPACK_ENC }, 4);
    default:
        return lsquic_frab_list_write(&qeh->qeh_fral,
                (unsigned char []) { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                        HQUST_QPACK_ENC }, 8);
    }
}