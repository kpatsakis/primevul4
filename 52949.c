static void read_marks(void)
{
	char line[512];
	FILE *f = fopen(import_marks_file, "r");
	if (f)
		;
	else if (import_marks_file_ignore_missing && errno == ENOENT)
		return; /* Marks file does not exist */
	else
		die_errno("cannot read '%s'", import_marks_file);
	while (fgets(line, sizeof(line), f)) {
		uintmax_t mark;
		char *end;
		unsigned char sha1[20];
		struct object_entry *e;

		end = strchr(line, '\n');
		if (line[0] != ':' || !end)
			die("corrupt mark line: %s", line);
		*end = 0;
		mark = strtoumax(line + 1, &end, 10);
		if (!mark || end == line + 1
			|| *end != ' ' || get_sha1_hex(end + 1, sha1))
			die("corrupt mark line: %s", line);
		e = find_object(sha1);
		if (!e) {
			enum object_type type = sha1_object_info(sha1, NULL);
			if (type < 0)
				die("object not found: %s", sha1_to_hex(sha1));
			e = insert_object(sha1);
			e->type = type;
			e->pack_id = MAX_PACK_ID;
			e->idx.offset = 1; /* just not zero! */
		}
		insert_mark(mark, e);
	}
	fclose(f);
}
