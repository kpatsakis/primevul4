real_crc32(unsigned long crc, const void *buff, size_t len)
{
	return crc32(crc, buff, (unsigned int)len);
}
