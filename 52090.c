void key_user_put(struct key_user *user)
{
	if (atomic_dec_and_lock(&user->usage, &key_user_lock)) {
		rb_erase(&user->node, &key_user_tree);
		spin_unlock(&key_user_lock);

		kfree(user);
	}
}
