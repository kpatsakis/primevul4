static struct object_entry *dereference(struct object_entry *oe,
					unsigned char sha1[20])
{
	unsigned long size;
	char *buf = NULL;
	if (!oe) {
		enum object_type type = sha1_object_info(sha1, NULL);
		if (type < 0)
			die("object not found: %s", sha1_to_hex(sha1));
		/* cache it! */
		oe = insert_object(sha1);
		oe->type = type;
		oe->pack_id = MAX_PACK_ID;
		oe->idx.offset = 1;
	}
	switch (oe->type) {
	case OBJ_TREE:	/* easy case. */
		return oe;
	case OBJ_COMMIT:
	case OBJ_TAG:
		break;
	default:
		die("Not a tree-ish: %s", command_buf.buf);
	}

	if (oe->pack_id != MAX_PACK_ID) {	/* in a pack being written */
		buf = gfi_unpack_entry(oe, &size);
	} else {
		enum object_type unused;
		buf = read_sha1_file(sha1, &unused, &size);
	}
	if (!buf)
		die("Can't load object %s", sha1_to_hex(sha1));

	/* Peel one layer. */
	switch (oe->type) {
	case OBJ_TAG:
		if (size < 40 + strlen("object ") ||
		    get_sha1_hex(buf + strlen("object "), sha1))
			die("Invalid SHA1 in tag: %s", command_buf.buf);
		break;
	case OBJ_COMMIT:
		if (size < 40 + strlen("tree ") ||
		    get_sha1_hex(buf + strlen("tree "), sha1))
			die("Invalid SHA1 in commit: %s", command_buf.buf);
	}

	free(buf);
	return find_object(sha1);
}
