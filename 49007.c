ssize_t rawexpwrite(off_t a, char *buf, size_t len, CLIENT *client, int fua) {
	int fhandle;
	off_t foffset;
	size_t maxbytes;
	ssize_t retval;

	if(get_filepos(client->export, a, &fhandle, &foffset, &maxbytes))
		return -1;
	if(maxbytes && len > maxbytes)
		len = maxbytes;

	DEBUG("(WRITE to fd %d offset %llu len %u fua %d), ", fhandle, (long long unsigned)foffset, (unsigned int)len, fua);

	myseek(fhandle, foffset);
	retval = write(fhandle, buf, len);
	if(client->server->flags & F_SYNC) {
		fsync(fhandle);
	} else if (fua) {

	  /* This is where we would do the following
	   *   #ifdef USE_SYNC_FILE_RANGE
	   * However, we don't, for the reasons set out below
	   * by Christoph Hellwig <hch@infradead.org>
	   *
	   * [BEGINS] 
	   * fdatasync is equivalent to fsync except that it does not flush
	   * non-essential metadata (basically just timestamps in practice), but it
	   * does flush metadata requried to find the data again, e.g. allocation
	   * information and extent maps.  sync_file_range does nothing but flush
	   * out pagecache content - it means you basically won't get your data
	   * back in case of a crash if you either:
	   * 
	   *  a) have a volatile write cache in your disk (e.g. any normal SATA disk)
	   *  b) are using a sparse file on a filesystem
	   *  c) are using a fallocate-preallocated file on a filesystem
	   *  d) use any file on a COW filesystem like btrfs
	   * 
	   * e.g. it only does anything useful for you if you do not have a volatile
	   * write cache, and either use a raw block device node, or just overwrite
	   * an already fully allocated (and not preallocated) file on a non-COW
	   * filesystem.
	   * [ENDS]
	   *
	   * What we should do is open a second FD with O_DSYNC set, then write to
	   * that when appropriate. However, with a Linux client, every REQ_FUA
	   * immediately follows a REQ_FLUSH, so fdatasync does not cause performance
	   * problems.
	   *
	   */
#if 0
		sync_file_range(fhandle, foffset, len,
				SYNC_FILE_RANGE_WAIT_BEFORE | SYNC_FILE_RANGE_WRITE |
				SYNC_FILE_RANGE_WAIT_AFTER);
#else
		fdatasync(fhandle);
#endif
	}
	return retval;
}
