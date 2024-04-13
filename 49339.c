static int crc32c_pcl_intel_update(struct shash_desc *desc, const u8 *data,
			       unsigned int len)
{
	u32 *crcp = shash_desc_ctx(desc);

	/*
	 * use faster PCL version if datasize is large enough to
	 * overcome kernel fpu state save/restore overhead
	 */
	if (len >= crc32c_pcl_breakeven && irq_fpu_usable()) {
		kernel_fpu_begin();
		*crcp = crc_pcl(data, len, *crcp);
		kernel_fpu_end();
	} else
		*crcp = crc32c_intel_le_hw(*crcp, data, len);
	return 0;
}
