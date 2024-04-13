static inline void consume(int f, void * buf, size_t len, size_t bufsiz) {
	size_t curlen;
	while (len>0) {
		curlen = (len>bufsiz)?bufsiz:len;
		readit(f, buf, curlen);
		len -= curlen;
	}
}
