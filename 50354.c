static int insert_links(struct ctl_table_header *head)
{
	struct ctl_table_set *root_set = &sysctl_table_root.default_set;
	struct ctl_dir *core_parent = NULL;
	struct ctl_table_header *links;
	int err;

	if (head->set == root_set)
		return 0;

	core_parent = xlate_dir(root_set, head->parent);
	if (IS_ERR(core_parent))
		return 0;

	if (get_links(core_parent, head->ctl_table, head->root))
		return 0;

	core_parent->header.nreg++;
	spin_unlock(&sysctl_lock);

	links = new_links(core_parent, head->ctl_table, head->root);

	spin_lock(&sysctl_lock);
	err = -ENOMEM;
	if (!links)
		goto out;

	err = 0;
	if (get_links(core_parent, head->ctl_table, head->root)) {
		kfree(links);
		goto out;
	}

	err = insert_header(core_parent, links);
	if (err)
		kfree(links);
out:
	drop_sysctl_table(&core_parent->header);
	return err;
}
