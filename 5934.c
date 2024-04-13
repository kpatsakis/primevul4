GF_Err ihdr_box_size(GF_Box *s)
{
	s->size += 14;
	return GF_OK;
}