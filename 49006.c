ssize_t rawexpread(off_t a, char *buf, size_t len, CLIENT *client) {
	int fhandle;
	off_t foffset;
	size_t maxbytes;

	if(get_filepos(client->export, a, &fhandle, &foffset, &maxbytes))
		return -1;
	if(maxbytes && len > maxbytes)
		len = maxbytes;

	DEBUG("(READ from fd %d offset %llu len %u), ", fhandle, (long long unsigned int)foffset, (unsigned int)len);

	myseek(fhandle, foffset);
	return read(fhandle, buf, len);
}
