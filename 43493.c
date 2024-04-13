void copy_time(struct timeval *dest, const struct timeval *src)
{
	memcpy(dest, src, sizeof(struct timeval));
}
