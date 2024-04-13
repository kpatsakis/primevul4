GF_Box *prft_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ProducerReferenceTimeBox, GF_ISOM_BOX_TYPE_PRFT);
	return (GF_Box *)tmp;
}