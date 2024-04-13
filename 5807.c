void isor_declare_pssh(ISOMChannel *ch)
{
	u32 i, PSSH_count;
	u8 *psshd;
	GF_BitStream *pssh_bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	u32 s;

	PSSH_count = gf_isom_get_pssh_count(ch->owner->mov);
	gf_bs_write_u32(pssh_bs, PSSH_count);

	/*fill PSSH in the structure. We will free it in CENC_Setup*/
	for (i=0; i<PSSH_count; i++) {
		bin128 SystemID;
		u32 version;
		u32 KID_count;
		bin128 *KIDs;
		u32 private_data_size;
		u8 *private_data;
		gf_isom_get_pssh_info(ch->owner->mov, i+1, SystemID, &version, &KID_count, (const bin128 **) & KIDs, (const u8 **) &private_data, &private_data_size);

		gf_bs_write_data(pssh_bs, SystemID, 16);
		gf_bs_write_u32(pssh_bs, version);
		gf_bs_write_u32(pssh_bs, KID_count);
		for (s=0; s<KID_count; s++) {
			gf_bs_write_data(pssh_bs, KIDs[s], 16);
		}
		gf_bs_write_u32(pssh_bs, private_data_size);
		gf_bs_write_data(pssh_bs, private_data, private_data_size);
	}
	gf_bs_get_content(pssh_bs, &psshd, &s);
	gf_bs_del(pssh_bs);
	gf_filter_pid_set_property(ch->pid, GF_PROP_PID_CENC_PSSH, & PROP_DATA_NO_COPY(psshd, s) );
}