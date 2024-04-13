GF_Err udta_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e = gf_isom_box_array_read(s, bs);
	if (e) return e;
	if (s->size==4) {
		u32 val = gf_bs_read_u32(bs);
		s->size = 0;
		if (val) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] udta has 4 remaining bytes set to %08X but they should be 0\n", val));
		}
	}
	return GF_OK;
}