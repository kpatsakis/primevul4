static void parse_from_commit(struct branch *b, char *buf, unsigned long size)
{
	if (!buf || size < 46)
		die("Not a valid commit: %s", sha1_to_hex(b->sha1));
	if (memcmp("tree ", buf, 5)
		|| get_sha1_hex(buf + 5, b->branch_tree.versions[1].sha1))
		die("The commit %s is corrupt", sha1_to_hex(b->sha1));
	hashcpy(b->branch_tree.versions[0].sha1,
		b->branch_tree.versions[1].sha1);
}
