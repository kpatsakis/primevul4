void big_key_free_preparse(struct key_preparsed_payload *prep)
{
	if (prep->datalen > BIG_KEY_FILE_THRESHOLD) {
		struct path *path = (struct path *)&prep->payload;
		path_put(path);
	} else {
		kfree(prep->payload[0]);
	}
}
