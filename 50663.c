static int fanout_set_data(struct packet_sock *po, char __user *data,
			   unsigned int len)
{
	switch (po->fanout->type) {
	case PACKET_FANOUT_CBPF:
		return fanout_set_data_cbpf(po, data, len);
	case PACKET_FANOUT_EBPF:
		return fanout_set_data_ebpf(po, data, len);
	default:
		return -EINVAL;
	};
}
