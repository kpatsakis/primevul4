static void __patch_reloc(RBuffer *buf, ut32 addr_to_patch, ut32 data_offset) {
	ut8 val[4] = { 0 };
	r_write_le32 (val, data_offset);
 	r_buf_write_at (buf, addr_to_patch, (void *)val, sizeof (val));
 }
