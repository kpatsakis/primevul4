int fsnotify(struct inode *to_tell, __u32 mask, const void *data, int data_is,
	     const unsigned char *file_name, u32 cookie)
{
	struct hlist_node *inode_node = NULL, *vfsmount_node = NULL;
	struct fsnotify_mark *inode_mark = NULL, *vfsmount_mark = NULL;
	struct fsnotify_group *inode_group, *vfsmount_group;
	struct fsnotify_mark_connector *inode_conn, *vfsmount_conn;
	struct fsnotify_iter_info iter_info;
	struct mount *mnt;
	int ret = 0;
	/* global tests shouldn't care about events on child only the specific event */
	__u32 test_mask = (mask & ~FS_EVENT_ON_CHILD);

	if (data_is == FSNOTIFY_EVENT_PATH)
		mnt = real_mount(((const struct path *)data)->mnt);
	else
		mnt = NULL;

	/*
	 * Optimization: srcu_read_lock() has a memory barrier which can
	 * be expensive.  It protects walking the *_fsnotify_marks lists.
	 * However, if we do not walk the lists, we do not have to do
	 * SRCU because we have no references to any objects and do not
	 * need SRCU to keep them "alive".
	 */
	if (!to_tell->i_fsnotify_marks &&
	    (!mnt || !mnt->mnt_fsnotify_marks))
		return 0;
	/*
	 * if this is a modify event we may need to clear the ignored masks
	 * otherwise return if neither the inode nor the vfsmount care about
	 * this type of event.
	 */
	if (!(mask & FS_MODIFY) &&
	    !(test_mask & to_tell->i_fsnotify_mask) &&
	    !(mnt && test_mask & mnt->mnt_fsnotify_mask))
		return 0;

	iter_info.srcu_idx = srcu_read_lock(&fsnotify_mark_srcu);

	if ((mask & FS_MODIFY) ||
	    (test_mask & to_tell->i_fsnotify_mask)) {
		inode_conn = srcu_dereference(to_tell->i_fsnotify_marks,
					      &fsnotify_mark_srcu);
		if (inode_conn)
			inode_node = srcu_dereference(inode_conn->list.first,
						      &fsnotify_mark_srcu);
	}

	if (mnt && ((mask & FS_MODIFY) ||
		    (test_mask & mnt->mnt_fsnotify_mask))) {
		inode_conn = srcu_dereference(to_tell->i_fsnotify_marks,
					      &fsnotify_mark_srcu);
		if (inode_conn)
			inode_node = srcu_dereference(inode_conn->list.first,
						      &fsnotify_mark_srcu);
		vfsmount_conn = srcu_dereference(mnt->mnt_fsnotify_marks,
					         &fsnotify_mark_srcu);
		if (vfsmount_conn)
			vfsmount_node = srcu_dereference(
						vfsmount_conn->list.first,
						&fsnotify_mark_srcu);
	}

	/*
	 * We need to merge inode & vfsmount mark lists so that inode mark
	 * ignore masks are properly reflected for mount mark notifications.
	 * That's why this traversal is so complicated...
	 */
	while (inode_node || vfsmount_node) {
		inode_group = NULL;
		inode_mark = NULL;
		vfsmount_group = NULL;
		vfsmount_mark = NULL;

		if (inode_node) {
			inode_mark = hlist_entry(srcu_dereference(inode_node, &fsnotify_mark_srcu),
						 struct fsnotify_mark, obj_list);
			inode_group = inode_mark->group;
		}

		if (vfsmount_node) {
			vfsmount_mark = hlist_entry(srcu_dereference(vfsmount_node, &fsnotify_mark_srcu),
						    struct fsnotify_mark, obj_list);
			vfsmount_group = vfsmount_mark->group;
		}

		if (inode_group && vfsmount_group) {
			int cmp = fsnotify_compare_groups(inode_group,
							  vfsmount_group);
			if (cmp > 0) {
				inode_group = NULL;
				inode_mark = NULL;
			} else if (cmp < 0) {
				vfsmount_group = NULL;
				vfsmount_mark = NULL;
			}
		}

		iter_info.inode_mark = inode_mark;
		iter_info.vfsmount_mark = vfsmount_mark;

		ret = send_to_group(to_tell, inode_mark, vfsmount_mark, mask,
				    data, data_is, cookie, file_name,
				    &iter_info);

		if (ret && (mask & ALL_FSNOTIFY_PERM_EVENTS))
			goto out;

		if (inode_group)
			inode_node = srcu_dereference(inode_node->next,
						      &fsnotify_mark_srcu);
		if (vfsmount_group)
			vfsmount_node = srcu_dereference(vfsmount_node->next,
							 &fsnotify_mark_srcu);
	}
	ret = 0;
out:
	srcu_read_unlock(&fsnotify_mark_srcu, iter_info.srcu_idx);

	return ret;
}
