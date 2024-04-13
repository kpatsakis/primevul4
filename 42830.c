static inline void rps_unlock(struct softnet_data *sd)
{
#ifdef CONFIG_RPS
	spin_unlock(&sd->input_pkt_queue.lock);
#endif
}
