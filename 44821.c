fst_attach(struct net_device *dev, unsigned short encoding, unsigned short parity)
{
	/*
	 * Setting currently fixed in FarSync card so we check and forget
	 */
	if (encoding != ENCODING_NRZ || parity != PARITY_CRC16_PR1_CCITT)
		return -EINVAL;
	return 0;
}
