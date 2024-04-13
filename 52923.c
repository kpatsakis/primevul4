static void parse_and_store_blob(
	struct last_object *last,
	unsigned char *sha1out,
	uintmax_t mark)
{
	static struct strbuf buf = STRBUF_INIT;
	uintmax_t len;

	if (parse_data(&buf, big_file_threshold, &len))
		store_object(OBJ_BLOB, &buf, last, sha1out, mark);
	else {
		if (last) {
			strbuf_release(&last->data);
			last->offset = 0;
			last->depth = 0;
		}
		stream_blob(len, sha1out, mark);
		skip_optional_lf();
	}
}
