static void isoffin_delete_channel(ISOMChannel *ch)
{
	isor_reset_reader(ch);
	if (ch->nal_bs) gf_bs_del(ch->nal_bs);
	if (ch->avcc) gf_odf_avc_cfg_del(ch->avcc);
	if (ch->hvcc) gf_odf_hevc_cfg_del(ch->hvcc);
	if (ch->vvcc) gf_odf_vvc_cfg_del(ch->vvcc);
	gf_free(ch);
}