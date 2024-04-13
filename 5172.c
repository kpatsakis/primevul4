GF_Err ftab_box_size(GF_Box *s)
{
	u32 i;
	GF_FontTableBox *ptr = (GF_FontTableBox *)s;

	s->size += 2;
	for (i=0; i<ptr->entry_count; i++) {
		s->size += 3;
		if (ptr->fonts[i].fontName) s->size += strlen(ptr->fonts[i].fontName);
	}
	return GF_OK;
}