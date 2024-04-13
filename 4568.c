GF_Err gf_isom_box_parse(GF_Box **outBox, GF_BitStream *bs)
{
	return gf_isom_box_parse_ex(outBox, bs, 0, GF_FALSE, 0);
}