compute_dh(unsigned char *buf, int size, BIGNUM *server_pub_key, DH *dh)
{
    int len, pad;

    len = DH_compute_key(buf, server_pub_key, dh);
    assert(len >= 0 && len <= size);
    if (len < size) {
        pad = size - len;
        memmove(buf + pad, buf, len);
        memset(buf, 0, pad);
    }
}
