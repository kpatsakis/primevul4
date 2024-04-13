static inline void rps_lock(struct softnet_data *sd)
{
#ifdef CONFIG_RPS
	spin_lock(&sd->input_pkt_queue.lock);
#endif
}
