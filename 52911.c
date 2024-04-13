static struct branch *new_branch(const char *name)
{
	unsigned int hc = hc_str(name, strlen(name)) % branch_table_sz;
	struct branch *b = lookup_branch(name);

	if (b)
		die("Invalid attempt to create duplicate branch: %s", name);
	if (check_refname_format(name, REFNAME_ALLOW_ONELEVEL))
		die("Branch name doesn't conform to GIT standards: %s", name);

	b = pool_calloc(1, sizeof(struct branch));
	b->name = pool_strdup(name);
	b->table_next_branch = branch_table[hc];
	b->branch_tree.versions[0].mode = S_IFDIR;
	b->branch_tree.versions[1].mode = S_IFDIR;
	b->num_notes = 0;
	b->active = 0;
	b->pack_id = MAX_PACK_ID;
	branch_table[hc] = b;
	branch_count++;
	return b;
}
