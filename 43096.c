xfs_da3_node_lookup_int(
	struct xfs_da_state	*state,
	int			*result)
{
	struct xfs_da_state_blk	*blk;
	struct xfs_da_blkinfo	*curr;
	struct xfs_da_intnode	*node;
	struct xfs_da_node_entry *btree;
	struct xfs_da3_icnode_hdr nodehdr;
	struct xfs_da_args	*args;
	xfs_dablk_t		blkno;
	xfs_dahash_t		hashval;
	xfs_dahash_t		btreehashval;
	int			probe;
	int			span;
	int			max;
	int			error;
	int			retval;
	struct xfs_inode	*dp = state->args->dp;

	args = state->args;

	/*
	 * Descend thru the B-tree searching each level for the right
	 * node to use, until the right hashval is found.
	 */
	blkno = (args->whichfork == XFS_DATA_FORK)? state->mp->m_dirleafblk : 0;
	for (blk = &state->path.blk[0], state->path.active = 1;
			 state->path.active <= XFS_DA_NODE_MAXDEPTH;
			 blk++, state->path.active++) {
		/*
		 * Read the next node down in the tree.
		 */
		blk->blkno = blkno;
		error = xfs_da3_node_read(args->trans, args->dp, blkno,
					-1, &blk->bp, args->whichfork);
		if (error) {
			blk->blkno = 0;
			state->path.active--;
			return(error);
		}
		curr = blk->bp->b_addr;
		blk->magic = be16_to_cpu(curr->magic);

		if (blk->magic == XFS_ATTR_LEAF_MAGIC ||
		    blk->magic == XFS_ATTR3_LEAF_MAGIC) {
			blk->magic = XFS_ATTR_LEAF_MAGIC;
			blk->hashval = xfs_attr_leaf_lasthash(blk->bp, NULL);
			break;
		}

		if (blk->magic == XFS_DIR2_LEAFN_MAGIC ||
		    blk->magic == XFS_DIR3_LEAFN_MAGIC) {
			blk->magic = XFS_DIR2_LEAFN_MAGIC;
			blk->hashval = xfs_dir2_leafn_lasthash(args->dp,
							       blk->bp, NULL);
			break;
		}

		blk->magic = XFS_DA_NODE_MAGIC;


		/*
		 * Search an intermediate node for a match.
		 */
		node = blk->bp->b_addr;
		dp->d_ops->node_hdr_from_disk(&nodehdr, node);
		btree = dp->d_ops->node_tree_p(node);

		max = nodehdr.count;
		blk->hashval = be32_to_cpu(btree[max - 1].hashval);

		/*
		 * Binary search.  (note: small blocks will skip loop)
		 */
		probe = span = max / 2;
		hashval = args->hashval;
		while (span > 4) {
			span /= 2;
			btreehashval = be32_to_cpu(btree[probe].hashval);
			if (btreehashval < hashval)
				probe += span;
			else if (btreehashval > hashval)
				probe -= span;
			else
				break;
		}
		ASSERT((probe >= 0) && (probe < max));
		ASSERT((span <= 4) ||
			(be32_to_cpu(btree[probe].hashval) == hashval));

		/*
		 * Since we may have duplicate hashval's, find the first
		 * matching hashval in the node.
		 */
		while (probe > 0 &&
		       be32_to_cpu(btree[probe].hashval) >= hashval) {
			probe--;
		}
		while (probe < max &&
		       be32_to_cpu(btree[probe].hashval) < hashval) {
			probe++;
		}

		/*
		 * Pick the right block to descend on.
		 */
		if (probe == max) {
			blk->index = max - 1;
			blkno = be32_to_cpu(btree[max - 1].before);
		} else {
			blk->index = probe;
			blkno = be32_to_cpu(btree[probe].before);
		}
	}

	/*
	 * A leaf block that ends in the hashval that we are interested in
	 * (final hashval == search hashval) means that the next block may
	 * contain more entries with the same hashval, shift upward to the
	 * next leaf and keep searching.
	 */
	for (;;) {
		if (blk->magic == XFS_DIR2_LEAFN_MAGIC) {
			retval = xfs_dir2_leafn_lookup_int(blk->bp, args,
							&blk->index, state);
		} else if (blk->magic == XFS_ATTR_LEAF_MAGIC) {
			retval = xfs_attr3_leaf_lookup_int(blk->bp, args);
			blk->index = args->index;
			args->blkno = blk->blkno;
		} else {
			ASSERT(0);
			return XFS_ERROR(EFSCORRUPTED);
		}
		if (((retval == ENOENT) || (retval == ENOATTR)) &&
		    (blk->hashval == args->hashval)) {
			error = xfs_da3_path_shift(state, &state->path, 1, 1,
							 &retval);
			if (error)
				return(error);
			if (retval == 0) {
				continue;
			} else if (blk->magic == XFS_ATTR_LEAF_MAGIC) {
				/* path_shift() gives ENOENT */
				retval = XFS_ERROR(ENOATTR);
			}
		}
		break;
	}
	*result = retval;
	return(0);
}
