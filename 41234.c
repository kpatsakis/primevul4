static struct mount *get_source(struct mount *dest,
				struct mount *last_dest,
				struct mount *last_src,
				int *type)
{
	struct mount *p_last_src = NULL;
	struct mount *p_last_dest = NULL;

	while (last_dest != dest->mnt_master) {
		p_last_dest = last_dest;
		p_last_src = last_src;
		last_dest = last_dest->mnt_master;
		last_src = last_src->mnt_master;
	}

	if (p_last_dest) {
		do {
			p_last_dest = next_peer(p_last_dest);
		} while (IS_MNT_NEW(p_last_dest));
		/* is that a peer of the earlier? */
		if (dest == p_last_dest) {
			*type = CL_MAKE_SHARED;
			return p_last_src;
		}
	}
	/* slave of the earlier, then */
	*type = CL_SLAVE;
	/* beginning of peer group among the slaves? */
	if (IS_MNT_SHARED(dest))
		*type |= CL_MAKE_SHARED;
	return last_src;
}
