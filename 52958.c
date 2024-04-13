static void stream_blob(uintmax_t len, unsigned char *sha1out, uintmax_t mark)
{
	size_t in_sz = 64 * 1024, out_sz = 64 * 1024;
	unsigned char *in_buf = xmalloc(in_sz);
	unsigned char *out_buf = xmalloc(out_sz);
	struct object_entry *e;
	unsigned char sha1[20];
	unsigned long hdrlen;
	off_t offset;
	git_SHA_CTX c;
	git_zstream s;
	struct sha1file_checkpoint checkpoint;
	int status = Z_OK;

	/* Determine if we should auto-checkpoint. */
	if ((max_packsize && (pack_size + 60 + len) > max_packsize)
		|| (pack_size + 60 + len) < pack_size)
		cycle_packfile();

	sha1file_checkpoint(pack_file, &checkpoint);
	offset = checkpoint.offset;

	hdrlen = snprintf((char *)out_buf, out_sz, "blob %" PRIuMAX, len) + 1;
	if (out_sz <= hdrlen)
		die("impossibly large object header");

	git_SHA1_Init(&c);
	git_SHA1_Update(&c, out_buf, hdrlen);

	crc32_begin(pack_file);

	git_deflate_init(&s, pack_compression_level);

	hdrlen = encode_in_pack_object_header(OBJ_BLOB, len, out_buf);
	if (out_sz <= hdrlen)
		die("impossibly large object header");

	s.next_out = out_buf + hdrlen;
	s.avail_out = out_sz - hdrlen;

	while (status != Z_STREAM_END) {
		if (0 < len && !s.avail_in) {
			size_t cnt = in_sz < len ? in_sz : (size_t)len;
			size_t n = fread(in_buf, 1, cnt, stdin);
			if (!n && feof(stdin))
				die("EOF in data (%" PRIuMAX " bytes remaining)", len);

			git_SHA1_Update(&c, in_buf, n);
			s.next_in = in_buf;
			s.avail_in = n;
			len -= n;
		}

		status = git_deflate(&s, len ? 0 : Z_FINISH);

		if (!s.avail_out || status == Z_STREAM_END) {
			size_t n = s.next_out - out_buf;
			sha1write(pack_file, out_buf, n);
			pack_size += n;
			s.next_out = out_buf;
			s.avail_out = out_sz;
		}

		switch (status) {
		case Z_OK:
		case Z_BUF_ERROR:
		case Z_STREAM_END:
			continue;
		default:
			die("unexpected deflate failure: %d", status);
		}
	}
	git_deflate_end(&s);
	git_SHA1_Final(sha1, &c);

	if (sha1out)
		hashcpy(sha1out, sha1);

	e = insert_object(sha1);

	if (mark)
		insert_mark(mark, e);

	if (e->idx.offset) {
		duplicate_count_by_type[OBJ_BLOB]++;
		truncate_pack(&checkpoint);

	} else if (find_sha1_pack(sha1, packed_git)) {
		e->type = OBJ_BLOB;
		e->pack_id = MAX_PACK_ID;
		e->idx.offset = 1; /* just not zero! */
		duplicate_count_by_type[OBJ_BLOB]++;
		truncate_pack(&checkpoint);

	} else {
		e->depth = 0;
		e->type = OBJ_BLOB;
		e->pack_id = pack_id;
		e->idx.offset = offset;
		e->idx.crc32 = crc32_end(pack_file);
		object_count++;
		object_count_by_type[OBJ_BLOB]++;
	}

	free(in_buf);
	free(out_buf);
}
