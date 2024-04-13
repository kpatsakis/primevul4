static inline const char * getcommandname(uint64_t command) {
	switch (command) {
	case NBD_CMD_READ:
		return "NBD_CMD_READ";
	case NBD_CMD_WRITE:
		return "NBD_CMD_WRITE";
	case NBD_CMD_DISC:
		return "NBD_CMD_DISC";
	case NBD_CMD_FLUSH:
		return "NBD_CMD_FLUSH";
	case NBD_CMD_TRIM:
		return "NBD_CMD_TRIM";
	default:
		return "UNKNOWN";
	}
}
