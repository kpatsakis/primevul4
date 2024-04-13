void isor_reset_reader(ISOMChannel *ch)
{
	ch->last_state = GF_OK;
	isor_reader_release_sample(ch);

	if (ch->static_sample) {
		ch->static_sample->dataLength = ch->static_sample->alloc_size;
		gf_isom_sample_del(&ch->static_sample);
	}
	ch->sample = NULL;
	ch->sample_num = 0;
	ch->speed = 1.0;
	ch->start = ch->end = 0;
	ch->to_init = 1;
	ch->playing = GF_FALSE;
	if (ch->sai_buffer) gf_free(ch->sai_buffer);
	ch->sai_buffer = NULL;
	ch->sai_alloc_size = 0;
	ch->dts = ch->cts = 0;
	ch->seek_flag = 0;
}