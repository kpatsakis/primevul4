GF_Box *blnk_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TextBlinkBox, GF_ISOM_BOX_TYPE_BLNK);
	return (GF_Box *) tmp;
}