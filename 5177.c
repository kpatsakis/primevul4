GF_Err dimC_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_DIMSSceneConfigBox *p = (GF_DIMSSceneConfigBox *)s;
	GF_Err e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u8(bs, p->profile);
	gf_bs_write_u8(bs, p->level);
	gf_bs_write_int(bs, p->pathComponents, 4);
	gf_bs_write_int(bs, p->fullRequestHost, 1);
	gf_bs_write_int(bs, p->streamType, 1);
	gf_bs_write_int(bs, p->containsRedundant, 2);
    if (p->textEncoding)
        gf_bs_write_data(bs, p->textEncoding, (u32) strlen(p->textEncoding));
    gf_bs_write_u8(bs, 0);
    if (p->contentEncoding)
        gf_bs_write_data(bs, p->contentEncoding, (u32) strlen(p->contentEncoding));
    gf_bs_write_u8(bs, 0);
	return GF_OK;
}