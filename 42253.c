int skb_copy_datagram_from_iovec(struct sk_buff *skb, int offset,
				 const struct iovec *from, int from_offset,
				 int len)
{
	int start = skb_headlen(skb);
	int i, copy = start - offset;
	struct sk_buff *frag_iter;

	/* Copy header. */
	if (copy > 0) {
		if (copy > len)
			copy = len;
		if (memcpy_fromiovecend(skb->data + offset, from, from_offset,
					copy))
			goto fault;
		if ((len -= copy) == 0)
			return 0;
		offset += copy;
		from_offset += copy;
	}

	/* Copy paged appendix. Hmm... why does this look so complicated? */
	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
		int end;
		const skb_frag_t *frag = &skb_shinfo(skb)->frags[i];

		WARN_ON(start > offset + len);

		end = start + skb_frag_size(frag);
		if ((copy = end - offset) > 0) {
			int err;
			u8  *vaddr;
			struct page *page = skb_frag_page(frag);

			if (copy > len)
				copy = len;
			vaddr = kmap(page);
			err = memcpy_fromiovecend(vaddr + frag->page_offset +
						  offset - start,
						  from, from_offset, copy);
			kunmap(page);
			if (err)
				goto fault;

			if (!(len -= copy))
				return 0;
			offset += copy;
			from_offset += copy;
		}
		start = end;
	}

	skb_walk_frags(skb, frag_iter) {
		int end;

		WARN_ON(start > offset + len);

		end = start + frag_iter->len;
		if ((copy = end - offset) > 0) {
			if (copy > len)
				copy = len;
			if (skb_copy_datagram_from_iovec(frag_iter,
							 offset - start,
							 from,
							 from_offset,
							 copy))
				goto fault;
			if ((len -= copy) == 0)
				return 0;
			offset += copy;
			from_offset += copy;
		}
		start = end;
	}
	if (!len)
		return 0;

fault:
	return -EFAULT;
}
