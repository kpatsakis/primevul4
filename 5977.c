GF_Err extr_box_size(GF_Box *s)
{
	GF_ExtraDataBox *ptr = (GF_ExtraDataBox *) s;
	ptr->size += ptr->data_length;
	if (ptr->feci) {
		GF_Err e = gf_isom_box_size((GF_Box*)ptr->feci);
		if (e) return e;
		ptr->size += ptr->feci->size;
	}
	return GF_OK;
}