GF_Err tx3g_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_Tx3gSampleEntryBox *ptr = (GF_Tx3gSampleEntryBox*)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_FTAB:
		BOX_FIELD_ASSIGN(font_table, GF_FontTableBox)
		break;
	default:
		return GF_OK;
	}
	return GF_OK;
}