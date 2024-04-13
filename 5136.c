void krok_box_del(GF_Box *s)
{
	GF_TextKaraokeBox*ptr = (GF_TextKaraokeBox*)s;
	if (ptr->records) gf_free(ptr->records);
	gf_free(ptr);
}