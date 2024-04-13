GF_Box *krok_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TextKaraokeBox, GF_ISOM_BOX_TYPE_KROK);
	return (GF_Box *) tmp;
}