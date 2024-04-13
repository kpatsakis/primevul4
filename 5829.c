void isor_reader_get_sample_from_item(ISOMChannel *ch)
{
	if (ch->au_seq_num) {
		if (!ch->owner->itt || !isor_declare_item_properties(ch->owner, ch, 1+ch->au_seq_num)) {
			ch->last_state = GF_EOS;
			return;
		}
	}
	ch->sample_time = 0;
	ch->last_state = GF_OK;
	if (!ch->static_sample) {
		ch->static_sample = gf_isom_sample_new();
	}

	ch->sample = ch->static_sample;
	ch->sample->IsRAP = RAP;
	ch->au_duration = 1000;
	ch->dts = ch->cts = 1000 * ch->au_seq_num;
	gf_isom_extract_meta_item_mem(ch->owner->mov, GF_TRUE, 0, ch->item_id, &ch->sample->data, &ch->sample->dataLength, &ch->static_sample->alloc_size, NULL, GF_FALSE);

	if (ch->is_encrypted && ch->is_cenc) {
		isor_update_cenc_info(ch, GF_TRUE);
	}
}