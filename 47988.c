local void writen(int desc, unsigned char *buf, size_t len)
{
    ssize_t ret;

    while (len) {
        ret = write(desc, buf, len);
        if (ret < 1) {
            complain("write error code %d", errno);
            bail("write error on ", g.outf);
        }
        buf += ret;
        len -= ret;
    }
}
