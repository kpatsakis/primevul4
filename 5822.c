static void isoffin_finalize(GF_Filter *filter)
{
	ISOMReader *read = (ISOMReader *) gf_filter_get_udta(filter);

	read->disconnected = GF_TRUE;

	while (gf_list_count(read->channels)) {
		ISOMChannel *ch = (ISOMChannel *)gf_list_get(read->channels, 0);
		gf_list_rem(read->channels, 0);
		isoffin_delete_channel(ch);
	}
	gf_list_del(read->channels);

	if (!read->extern_mov && read->mov) gf_isom_close(read->mov);
	read->mov = NULL;

	if (read->mem_blob.data) gf_free(read->mem_blob.data);
	if (read->mem_url) gf_free(read->mem_url);
}