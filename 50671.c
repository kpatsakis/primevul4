static void packet_cached_dev_reset(struct packet_sock *po)
{
	RCU_INIT_POINTER(po->cached_dev, NULL);
}
