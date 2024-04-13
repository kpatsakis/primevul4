static int __chksum_finup(__u16 *crcp, const u8 *data, unsigned int len,
			u8 *out)
{
	if (irq_fpu_usable()) {
		kernel_fpu_begin();
		*(__u16 *)out = crc_t10dif_pcl(*crcp, data, len);
		kernel_fpu_end();
	} else
		*(__u16 *)out = crc_t10dif_generic(*crcp, data, len);
	return 0;
}
