void href_box_del(GF_Box *s)
{
	GF_TextHyperTextBox*ptr = (GF_TextHyperTextBox*)s;
	if (ptr->URL) gf_free(ptr->URL);
	if (ptr->URL_hint) gf_free(ptr->URL_hint);
	gf_free(ptr);
}