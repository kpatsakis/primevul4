static void sched_feat_enable(int i)
{
	if (!static_key_enabled(&sched_feat_keys[i]))
		static_key_slow_inc(&sched_feat_keys[i]);
}
