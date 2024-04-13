GF_Box *clap_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_CleanApertureBox, GF_ISOM_BOX_TYPE_CLAP);
	return (GF_Box *)tmp;
}