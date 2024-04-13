static int __chksum_finup(__u16 *crcp, const u8 *data, unsigned int len,
			u8 *out)
{
	*(__u16 *)out = crc_t10dif_generic(*crcp, data, len);
	return 0;
}
