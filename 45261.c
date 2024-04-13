static inline int ieee802154_match_sock(u8 *hw_addr, u16 pan_id,
		u16 short_addr, struct dgram_sock *ro)
{
	if (!ro->bound)
		return 1;

	if (ro->src_addr.addr_type == IEEE802154_ADDR_LONG &&
	    !memcmp(ro->src_addr.hwaddr, hw_addr, IEEE802154_ADDR_LEN))
		return 1;

	if (ro->src_addr.addr_type == IEEE802154_ADDR_SHORT &&
		     pan_id == ro->src_addr.pan_id &&
		     short_addr == ro->src_addr.short_addr)
		return 1;

	return 0;
}
