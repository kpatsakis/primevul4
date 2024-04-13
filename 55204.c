struct snd_seq_subscribers *snd_seq_port_get_subscription(struct snd_seq_port_subs_info *src_grp,
							  struct snd_seq_addr *dest_addr)
{
	struct snd_seq_subscribers *s, *found = NULL;

	down_read(&src_grp->list_mutex);
	list_for_each_entry(s, &src_grp->list_head, src_list) {
		if (addr_match(dest_addr, &s->info.dest)) {
			found = s;
			break;
		}
	}
	up_read(&src_grp->list_mutex);
	return found;
}
