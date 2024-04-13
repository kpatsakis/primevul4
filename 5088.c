static bool check_buffer(struct torture_context *tctx,
			 uint8_t *buf, unsigned int seed, int len, const char *location)
{
	int i;
	srandom(seed);
	for (i=0;i<len;i++) {
		uint8_t v = random();
		if (buf[i] != v) {
			torture_fail(tctx, talloc_asprintf(tctx, "Buffer incorrect at %s! ofs=%d buf=0x%x correct=0x%x\n",
			       location, i, buf[i], v));
			return false;
		}
	}
	return true;
}