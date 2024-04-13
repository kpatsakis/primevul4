GF_Err krok_box_size(GF_Box *s)
{
	GF_TextKaraokeBox*ptr = (GF_TextKaraokeBox*)s;
	s->size += 6 + 8*ptr->nb_entries;
	return GF_OK;
}