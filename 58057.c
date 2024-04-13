static void catc_multicast(unsigned char *addr, u8 *multicast)
{
	u32 crc;

	crc = ether_crc_le(6, addr);
	multicast[(crc >> 3) & 0x3f] |= 1 << (crc & 7);
}
