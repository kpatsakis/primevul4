GF_Err href_box_size(GF_Box *s)
{
	GF_TextHyperTextBox*ptr = (GF_TextHyperTextBox*)s;
	s->size += 6;
	if (ptr->URL) s->size += strlen(ptr->URL);
	if (ptr->URL_hint) s->size += strlen(ptr->URL_hint);
	return GF_OK;
}