static void cat_blob(struct object_entry *oe, unsigned char sha1[20])
{
	struct strbuf line = STRBUF_INIT;
	unsigned long size;
	enum object_type type = 0;
	char *buf;

	if (!oe || oe->pack_id == MAX_PACK_ID) {
		buf = read_sha1_file(sha1, &type, &size);
	} else {
		type = oe->type;
		buf = gfi_unpack_entry(oe, &size);
	}

	/*
	 * Output based on batch_one_object() from cat-file.c.
	 */
	if (type <= 0) {
		strbuf_reset(&line);
		strbuf_addf(&line, "%s missing\n", sha1_to_hex(sha1));
		cat_blob_write(line.buf, line.len);
		strbuf_release(&line);
		free(buf);
		return;
	}
	if (!buf)
		die("Can't read object %s", sha1_to_hex(sha1));
	if (type != OBJ_BLOB)
		die("Object %s is a %s but a blob was expected.",
		    sha1_to_hex(sha1), typename(type));
	strbuf_reset(&line);
	strbuf_addf(&line, "%s %s %lu\n", sha1_to_hex(sha1),
						typename(type), size);
	cat_blob_write(line.buf, line.len);
	strbuf_release(&line);
	cat_blob_write(buf, size);
	cat_blob_write("\n", 1);
	if (oe && oe->pack_id == pack_id) {
		last_blob.offset = oe->idx.offset;
		strbuf_attach(&last_blob.data, buf, size, size);
		last_blob.depth = oe->depth;
	} else
		free(buf);
}
