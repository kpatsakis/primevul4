static u32 atl2_hash_mc_addr(struct atl2_hw *hw, u8 *mc_addr)
{
	u32 crc32, value;
	int i;

	value = 0;
	crc32 = ether_crc_le(6, mc_addr);

	for (i = 0; i < 32; i++)
		value |= (((crc32 >> i) & 1) << (31 - i));

	return value;
}
