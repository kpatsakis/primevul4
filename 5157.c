void styl_box_del(GF_Box *s)
{
	GF_TextStyleBox*ptr = (GF_TextStyleBox*)s;
	if (ptr->styles) gf_free(ptr->styles);
	gf_free(ptr);
}