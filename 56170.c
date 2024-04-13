ts_unix_format(netdissect_options *ndo
#ifndef HAVE_PCAP_SET_TSTAMP_PRECISION
_U_
#endif
, int sec, int usec, char *buf)
{
	const char *format;

#ifdef HAVE_PCAP_SET_TSTAMP_PRECISION
	switch (ndo->ndo_tstamp_precision) {

	case PCAP_TSTAMP_PRECISION_MICRO:
		format = "%u.%06u";
		break;

	case PCAP_TSTAMP_PRECISION_NANO:
		format = "%u.%09u";
		break;

	default:
		format = "%u.{unknown}";
		break;
	}
#else
	format = "%u.%06u";
#endif

	snprintf(buf, TS_BUF_SIZE, format,
		 (unsigned)sec, (unsigned)usec);

	return buf;
}
