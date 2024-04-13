static u32 __crc32_le(u32 crc, unsigned char const *p, size_t len)
{
	return crc32_le(crc, p, len);
}
