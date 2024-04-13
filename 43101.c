xfs_da3_node_split(
	struct xfs_da_state	*state,
	struct xfs_da_state_blk	*oldblk,
	struct xfs_da_state_blk	*newblk,
	struct xfs_da_state_blk	*addblk,
	int			treelevel,
	int			*result)
{
	struct xfs_da_intnode	*node;
	struct xfs_da3_icnode_hdr nodehdr;
	xfs_dablk_t		blkno;
	int			newcount;
	int			error;
	int			useextra;
	struct xfs_inode	*dp = state->args->dp;

	trace_xfs_da_node_split(state->args);

	node = oldblk->bp->b_addr;
	dp->d_ops->node_hdr_from_disk(&nodehdr, node);

	/*
	 * With V2 dirs the extra block is data or freespace.
	 */
	useextra = state->extravalid && state->args->whichfork == XFS_ATTR_FORK;
	newcount = 1 + useextra;
	/*
	 * Do we have to split the node?
	 */
	if (nodehdr.count + newcount > state->node_ents) {
		/*
		 * Allocate a new node, add to the doubly linked chain of
		 * nodes, then move some of our excess entries into it.
		 */
		error = xfs_da_grow_inode(state->args, &blkno);
		if (error)
			return(error);	/* GROT: dir is inconsistent */

		error = xfs_da3_node_create(state->args, blkno, treelevel,
					   &newblk->bp, state->args->whichfork);
		if (error)
			return(error);	/* GROT: dir is inconsistent */
		newblk->blkno = blkno;
		newblk->magic = XFS_DA_NODE_MAGIC;
		xfs_da3_node_rebalance(state, oldblk, newblk);
		error = xfs_da3_blk_link(state, oldblk, newblk);
		if (error)
			return(error);
		*result = 1;
	} else {
		*result = 0;
	}

	/*
	 * Insert the new entry(s) into the correct block
	 * (updating last hashval in the process).
	 *
	 * xfs_da3_node_add() inserts BEFORE the given index,
	 * and as a result of using node_lookup_int() we always
	 * point to a valid entry (not after one), but a split
	 * operation always results in a new block whose hashvals
	 * FOLLOW the current block.
	 *
	 * If we had double-split op below us, then add the extra block too.
	 */
	node = oldblk->bp->b_addr;
	dp->d_ops->node_hdr_from_disk(&nodehdr, node);
	if (oldblk->index <= nodehdr.count) {
		oldblk->index++;
		xfs_da3_node_add(state, oldblk, addblk);
		if (useextra) {
			if (state->extraafter)
				oldblk->index++;
			xfs_da3_node_add(state, oldblk, &state->extrablk);
			state->extravalid = 0;
		}
	} else {
		newblk->index++;
		xfs_da3_node_add(state, newblk, addblk);
		if (useextra) {
			if (state->extraafter)
				newblk->index++;
			xfs_da3_node_add(state, newblk, &state->extrablk);
			state->extravalid = 0;
		}
	}

	return(0);
}
