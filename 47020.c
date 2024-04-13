static unsigned int get_mergeable_buf_len(struct ewma *avg_pkt_len)
{
	const size_t hdr_len = sizeof(struct virtio_net_hdr_mrg_rxbuf);
	unsigned int len;

	len = hdr_len + clamp_t(unsigned int, ewma_read(avg_pkt_len),
			GOOD_PACKET_LEN, PAGE_SIZE - hdr_len);
	return ALIGN(len, MERGEABLE_BUFFER_ALIGN);
}
