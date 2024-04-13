ts_format(netdissect_options *ndo
#ifndef HAVE_PCAP_SET_TSTAMP_PRECISION
_U_
#endif
, int sec, int usec, char *buf)
{
	const char *format;

#ifdef HAVE_PCAP_SET_TSTAMP_PRECISION
	switch (ndo->ndo_tstamp_precision) {

	case PCAP_TSTAMP_PRECISION_MICRO:
		format = "%02d:%02d:%02d.%06u";
		break;

	case PCAP_TSTAMP_PRECISION_NANO:
		format = "%02d:%02d:%02d.%09u";
		break;

	default:
		format = "%02d:%02d:%02d.{unknown}";
		break;
	}
#else
	format = "%02d:%02d:%02d.%06u";
#endif

	snprintf(buf, TS_BUF_SIZE, format,
                 sec / 3600, (sec % 3600) / 60, sec % 60, usec);

        return buf;
}
