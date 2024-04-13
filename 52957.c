static int store_object(
	enum object_type type,
	struct strbuf *dat,
	struct last_object *last,
	unsigned char *sha1out,
	uintmax_t mark)
{
	void *out, *delta;
	struct object_entry *e;
	unsigned char hdr[96];
	unsigned char sha1[20];
	unsigned long hdrlen, deltalen;
	git_SHA_CTX c;
	git_zstream s;

	hdrlen = xsnprintf((char *)hdr, sizeof(hdr), "%s %lu",
			   typename(type), (unsigned long)dat->len) + 1;
	git_SHA1_Init(&c);
	git_SHA1_Update(&c, hdr, hdrlen);
	git_SHA1_Update(&c, dat->buf, dat->len);
	git_SHA1_Final(sha1, &c);
	if (sha1out)
		hashcpy(sha1out, sha1);

	e = insert_object(sha1);
	if (mark)
		insert_mark(mark, e);
	if (e->idx.offset) {
		duplicate_count_by_type[type]++;
		return 1;
	} else if (find_sha1_pack(sha1, packed_git)) {
		e->type = type;
		e->pack_id = MAX_PACK_ID;
		e->idx.offset = 1; /* just not zero! */
		duplicate_count_by_type[type]++;
		return 1;
	}

	if (last && last->data.buf && last->depth < max_depth && dat->len > 20) {
		delta_count_attempts_by_type[type]++;
		delta = diff_delta(last->data.buf, last->data.len,
			dat->buf, dat->len,
			&deltalen, dat->len - 20);
	} else
		delta = NULL;

	git_deflate_init(&s, pack_compression_level);
	if (delta) {
		s.next_in = delta;
		s.avail_in = deltalen;
	} else {
		s.next_in = (void *)dat->buf;
		s.avail_in = dat->len;
	}
	s.avail_out = git_deflate_bound(&s, s.avail_in);
	s.next_out = out = xmalloc(s.avail_out);
	while (git_deflate(&s, Z_FINISH) == Z_OK)
		; /* nothing */
	git_deflate_end(&s);

	/* Determine if we should auto-checkpoint. */
	if ((max_packsize && (pack_size + 60 + s.total_out) > max_packsize)
		|| (pack_size + 60 + s.total_out) < pack_size) {

		/* This new object needs to *not* have the current pack_id. */
		e->pack_id = pack_id + 1;
		cycle_packfile();

		/* We cannot carry a delta into the new pack. */
		if (delta) {
			free(delta);
			delta = NULL;

			git_deflate_init(&s, pack_compression_level);
			s.next_in = (void *)dat->buf;
			s.avail_in = dat->len;
			s.avail_out = git_deflate_bound(&s, s.avail_in);
			s.next_out = out = xrealloc(out, s.avail_out);
			while (git_deflate(&s, Z_FINISH) == Z_OK)
				; /* nothing */
			git_deflate_end(&s);
		}
	}

	e->type = type;
	e->pack_id = pack_id;
	e->idx.offset = pack_size;
	object_count++;
	object_count_by_type[type]++;

	crc32_begin(pack_file);

	if (delta) {
		off_t ofs = e->idx.offset - last->offset;
		unsigned pos = sizeof(hdr) - 1;

		delta_count_by_type[type]++;
		e->depth = last->depth + 1;

		hdrlen = encode_in_pack_object_header(OBJ_OFS_DELTA, deltalen, hdr);
		sha1write(pack_file, hdr, hdrlen);
		pack_size += hdrlen;

		hdr[pos] = ofs & 127;
		while (ofs >>= 7)
			hdr[--pos] = 128 | (--ofs & 127);
		sha1write(pack_file, hdr + pos, sizeof(hdr) - pos);
		pack_size += sizeof(hdr) - pos;
	} else {
		e->depth = 0;
		hdrlen = encode_in_pack_object_header(type, dat->len, hdr);
		sha1write(pack_file, hdr, hdrlen);
		pack_size += hdrlen;
	}

	sha1write(pack_file, out, s.total_out);
	pack_size += s.total_out;

	e->idx.crc32 = crc32_end(pack_file);

	free(out);
	free(delta);
	if (last) {
		if (last->no_swap) {
			last->data = *dat;
		} else {
			strbuf_swap(&last->data, dat);
		}
		last->offset = e->idx.offset;
		last->depth = e->depth;
	}
	return 0;
}
