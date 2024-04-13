static void start_packfile(void)
{
	static char tmp_file[PATH_MAX];
	struct packed_git *p;
	int namelen;
	struct pack_header hdr;
	int pack_fd;

	pack_fd = odb_mkstemp(tmp_file, sizeof(tmp_file),
			      "pack/tmp_pack_XXXXXX");
	namelen = strlen(tmp_file) + 2;
	p = xcalloc(1, sizeof(*p) + namelen);
	xsnprintf(p->pack_name, namelen, "%s", tmp_file);
	p->pack_fd = pack_fd;
	p->do_not_close = 1;
	pack_file = sha1fd(pack_fd, p->pack_name);

	hdr.hdr_signature = htonl(PACK_SIGNATURE);
	hdr.hdr_version = htonl(2);
	hdr.hdr_entries = 0;
	sha1write(pack_file, &hdr, sizeof(hdr));

	pack_data = p;
	pack_size = sizeof(hdr);
	object_count = 0;

	REALLOC_ARRAY(all_packs, pack_id + 1);
	all_packs[pack_id] = p;
}
