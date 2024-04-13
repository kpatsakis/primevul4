ieee8021q_tci_string(const uint16_t tci)
{
	static char buf[128];
	snprintf(buf, sizeof(buf), "vlan %u, p %u%s",
	         tci & 0xfff,
	         tci >> 13,
	         (tci & 0x1000) ? ", DEI" : "");
	return buf;
}
