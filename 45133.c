cifs_direct_io(int rw, struct kiocb *iocb, const struct iovec *iov,
               loff_t pos, unsigned long nr_segs)
{
        /*
         * FIXME
         * Eventually need to support direct IO for non forcedirectio mounts
         */
        return -EINVAL;
}
