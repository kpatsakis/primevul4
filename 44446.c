static int ath_max_framelen(int usec, int mcs, bool ht40, bool sgi)
{
	int streams = HT_RC_2_STREAMS(mcs);
	int symbols, bits;
	int bytes = 0;

	symbols = sgi ? TIME_SYMBOLS_HALFGI(usec) : TIME_SYMBOLS(usec);
	bits = symbols * bits_per_symbol[mcs % 8][ht40] * streams;
	bits -= OFDM_PLCP_BITS;
	bytes = bits / 8;
	bytes -= L_STF + L_LTF + L_SIG + HT_SIG + HT_STF + HT_LTF(streams);
	if (bytes > 65532)
		bytes = 65532;

	return bytes;
}
