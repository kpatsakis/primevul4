static void defense_work_handler(struct work_struct *work)
{
	struct netns_ipvs *ipvs =
		container_of(work, struct netns_ipvs, defense_work.work);

	update_defense_level(ipvs);
	if (atomic_read(&ipvs->dropentry))
		ip_vs_random_dropentry(ipvs->net);
	schedule_delayed_work(&ipvs->defense_work, DEFENSE_TIMER_PERIOD);
}
