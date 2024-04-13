GF_Err styl_box_size(GF_Box *s)
{
	GF_TextStyleBox*ptr = (GF_TextStyleBox*)s;

	s->size += 2 + ptr->entry_count * GPP_STYLE_SIZE;
	return GF_OK;
}