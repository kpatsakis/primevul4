static void isoffin_disconnect(ISOMReader *read)
{
	read->disconnected = GF_TRUE;
	while (gf_list_count(read->channels)) {
		ISOMChannel *ch = (ISOMChannel *)gf_list_get(read->channels, 0);
		gf_list_rem(read->channels, 0);
		if (ch->pid)
			gf_filter_pid_remove(ch->pid);
		isoffin_delete_channel(ch);
	}

	if (read->mov) gf_isom_close(read->mov);
	read->mov = NULL;

	read->pid = NULL;
}