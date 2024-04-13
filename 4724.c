void fmtutil_riscos_read_load_exec(deark *c, dbuf *f, struct de_riscos_file_attrs *rfa, i64 pos1)
{
	i64 pos = pos1;

	rfa->load_addr = (u32)dbuf_getu32le_p(f, &pos);
	rfa->exec_addr = (u32)dbuf_getu32le_p(f, &pos);
	de_dbg(c, "load/exec addrs: 0x%08x, 0x%08x", (unsigned int)rfa->load_addr,
		(unsigned int)rfa->exec_addr);
	de_dbg_indent(c, 1);
	if((rfa->load_addr&0xfff00000U)==0xfff00000U) {
		rfa->file_type = (unsigned int)((rfa->load_addr&0xfff00)>>8);
		rfa->file_type_known = 1;
		de_dbg(c, "file type: %03X", rfa->file_type);

		de_riscos_loadexec_to_timestamp(rfa->load_addr, rfa->exec_addr, &rfa->mod_time);
		dbg_timestamp(c, &rfa->mod_time, "timestamp");
	}
	de_dbg_indent(c, -1);
}