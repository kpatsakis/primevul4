static void get_futex_key_refs(union futex_key *key)
{
	if (!key->both.ptr)
		return;

	switch (key->both.offset & (FUT_OFF_INODE|FUT_OFF_MMSHARED)) {
	case FUT_OFF_INODE:
		ihold(key->shared.inode);
		break;
	case FUT_OFF_MMSHARED:
		atomic_inc(&key->private.mm->mm_count);
		break;
	}
}
