static uintmax_t change_note_fanout(struct tree_entry *root,
		unsigned char fanout)
{
	char hex_sha1[40], path[60];
	return do_change_note_fanout(root, root, hex_sha1, 0, path, 0, fanout);
}
