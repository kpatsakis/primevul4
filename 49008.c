static inline void readit(int f, void *buf, size_t len) {
	ssize_t res;
	while (len > 0) {
		DEBUG("*");
		if ((res = read(f, buf, len)) <= 0) {
			if(errno != EAGAIN) {
				err("Read failed: %m");
			}
		} else {
			len -= res;
			buf += res;
		}
	}
}
