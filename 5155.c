void ftab_box_del(GF_Box *s)
{
	GF_FontTableBox *ptr = (GF_FontTableBox *)s;
	if (ptr->fonts) {
		u32 i;
		for (i=0; i<ptr->entry_count; i++)
			if (ptr->fonts[i].fontName) gf_free(ptr->fonts[i].fontName);
		gf_free(ptr->fonts);
	}
	gf_free(ptr);
}