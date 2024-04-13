struct key_user *key_user_lookup(kuid_t uid)
{
	struct key_user *candidate = NULL, *user;
	struct rb_node *parent = NULL;
	struct rb_node **p;

try_again:
	p = &key_user_tree.rb_node;
	spin_lock(&key_user_lock);

	/* search the tree for a user record with a matching UID */
	while (*p) {
		parent = *p;
		user = rb_entry(parent, struct key_user, node);

		if (uid_lt(uid, user->uid))
			p = &(*p)->rb_left;
		else if (uid_gt(uid, user->uid))
			p = &(*p)->rb_right;
		else
			goto found;
	}

	/* if we get here, we failed to find a match in the tree */
	if (!candidate) {
		/* allocate a candidate user record if we don't already have
		 * one */
		spin_unlock(&key_user_lock);

		user = NULL;
		candidate = kmalloc(sizeof(struct key_user), GFP_KERNEL);
		if (unlikely(!candidate))
			goto out;

		/* the allocation may have scheduled, so we need to repeat the
		 * search lest someone else added the record whilst we were
		 * asleep */
		goto try_again;
	}

	/* if we get here, then the user record still hadn't appeared on the
	 * second pass - so we use the candidate record */
	atomic_set(&candidate->usage, 1);
	atomic_set(&candidate->nkeys, 0);
	atomic_set(&candidate->nikeys, 0);
	candidate->uid = uid;
	candidate->qnkeys = 0;
	candidate->qnbytes = 0;
	spin_lock_init(&candidate->lock);
	mutex_init(&candidate->cons_lock);

	rb_link_node(&candidate->node, parent, p);
	rb_insert_color(&candidate->node, &key_user_tree);
	spin_unlock(&key_user_lock);
	user = candidate;
	goto out;

	/* okay - we found a user record for this UID */
found:
	atomic_inc(&user->usage);
	spin_unlock(&key_user_lock);
	kfree(candidate);
out:
	return user;
}
