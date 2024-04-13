void isor_reader_release_sample(ISOMChannel *ch)
{
	if (ch->sample)
		ch->au_seq_num++;
	ch->sample = NULL;
	ch->sai_buffer_size = 0;
}