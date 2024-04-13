static void tree_content_replace(
	struct tree_entry *root,
	const unsigned char *sha1,
	const uint16_t mode,
	struct tree_content *newtree)
{
	if (!S_ISDIR(mode))
		die("Root cannot be a non-directory");
	hashclr(root->versions[0].sha1);
	hashcpy(root->versions[1].sha1, sha1);
	if (root->tree)
		release_tree_content_recursive(root->tree);
	root->tree = newtree;
}
