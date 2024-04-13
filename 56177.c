data_header_print(netdissect_options *ndo, uint16_t fc, const u_char *p)
{
	u_int subtype = FC_SUBTYPE(fc);

	if (DATA_FRAME_IS_CF_ACK(subtype) || DATA_FRAME_IS_CF_POLL(subtype) ||
	    DATA_FRAME_IS_QOS(subtype)) {
		ND_PRINT((ndo, "CF "));
		if (DATA_FRAME_IS_CF_ACK(subtype)) {
			if (DATA_FRAME_IS_CF_POLL(subtype))
				ND_PRINT((ndo, "Ack/Poll"));
			else
				ND_PRINT((ndo, "Ack"));
		} else {
			if (DATA_FRAME_IS_CF_POLL(subtype))
				ND_PRINT((ndo, "Poll"));
		}
		if (DATA_FRAME_IS_QOS(subtype))
			ND_PRINT((ndo, "+QoS"));
		ND_PRINT((ndo, " "));
	}

#define ADDR1  (p + 4)
#define ADDR2  (p + 10)
#define ADDR3  (p + 16)
#define ADDR4  (p + 24)

	if (!FC_TO_DS(fc) && !FC_FROM_DS(fc)) {
		ND_PRINT((ndo, "DA:%s SA:%s BSSID:%s ",
		    etheraddr_string(ndo, ADDR1), etheraddr_string(ndo, ADDR2),
		    etheraddr_string(ndo, ADDR3)));
	} else if (!FC_TO_DS(fc) && FC_FROM_DS(fc)) {
		ND_PRINT((ndo, "DA:%s BSSID:%s SA:%s ",
		    etheraddr_string(ndo, ADDR1), etheraddr_string(ndo, ADDR2),
		    etheraddr_string(ndo, ADDR3)));
	} else if (FC_TO_DS(fc) && !FC_FROM_DS(fc)) {
		ND_PRINT((ndo, "BSSID:%s SA:%s DA:%s ",
		    etheraddr_string(ndo, ADDR1), etheraddr_string(ndo, ADDR2),
		    etheraddr_string(ndo, ADDR3)));
	} else if (FC_TO_DS(fc) && FC_FROM_DS(fc)) {
		ND_PRINT((ndo, "RA:%s TA:%s DA:%s SA:%s ",
		    etheraddr_string(ndo, ADDR1), etheraddr_string(ndo, ADDR2),
		    etheraddr_string(ndo, ADDR3), etheraddr_string(ndo, ADDR4)));
	}

#undef ADDR1
#undef ADDR2
#undef ADDR3
#undef ADDR4
}
