get_subscriber(struct list_head *p, bool is_src)
{
	if (is_src)
		return list_entry(p, struct snd_seq_subscribers, src_list);
	else
		return list_entry(p, struct snd_seq_subscribers, dest_list);
}
