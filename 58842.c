static bool __skb_splice_bits(struct sk_buff *skb, struct pipe_inode_info *pipe,
			      unsigned int *offset, unsigned int *len,
			      struct splice_pipe_desc *spd, struct sock *sk)
{
	int seg;
	struct sk_buff *iter;

	/* map the linear part :
	 * If skb->head_frag is set, this 'linear' part is backed by a
	 * fragment, and if the head is not shared with any clones then
	 * we can avoid a copy since we own the head portion of this page.
	 */
	if (__splice_segment(virt_to_page(skb->data),
			     (unsigned long) skb->data & (PAGE_SIZE - 1),
			     skb_headlen(skb),
			     offset, len, spd,
			     skb_head_is_locked(skb),
			     sk, pipe))
		return true;

	/*
	 * then map the fragments
	 */
	for (seg = 0; seg < skb_shinfo(skb)->nr_frags; seg++) {
		const skb_frag_t *f = &skb_shinfo(skb)->frags[seg];

		if (__splice_segment(skb_frag_page(f),
				     f->page_offset, skb_frag_size(f),
				     offset, len, spd, false, sk, pipe))
			return true;
	}

	skb_walk_frags(skb, iter) {
		if (*offset >= iter->len) {
			*offset -= iter->len;
			continue;
		}
		/* __skb_splice_bits() only fails if the output has no room
		 * left, so no point in going over the frag_list for the error
		 * case.
		 */
		if (__skb_splice_bits(iter, pipe, offset, len, spd, sk))
			return true;
	}

	return false;
}
