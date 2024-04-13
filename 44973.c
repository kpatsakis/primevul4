static int path_walk(const char *name, struct nameidata *nd)
{
	struct path save = nd->path;
	int result;

	current->total_link_count = 0;

	/* make sure the stuff we saved doesn't go away */
	path_get(&save);

	result = link_path_walk(name, nd);
	if (result == -ESTALE) {
		/* nd->path had been dropped */
		current->total_link_count = 0;
		nd->path = save;
		path_get(&nd->path);
		nd->flags |= LOOKUP_REVAL;
		result = link_path_walk(name, nd);
	}

	path_put(&save);

	return result;
}
