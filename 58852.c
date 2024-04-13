static int pskb_carve_frag_list(struct sk_buff *skb,
				struct skb_shared_info *shinfo, int eat,
				gfp_t gfp_mask)
{
	struct sk_buff *list = shinfo->frag_list;
	struct sk_buff *clone = NULL;
	struct sk_buff *insp = NULL;

	do {
		if (!list) {
			pr_err("Not enough bytes to eat. Want %d\n", eat);
			return -EFAULT;
		}
		if (list->len <= eat) {
			/* Eaten as whole. */
			eat -= list->len;
			list = list->next;
			insp = list;
		} else {
			/* Eaten partially. */
			if (skb_shared(list)) {
				clone = skb_clone(list, gfp_mask);
				if (!clone)
					return -ENOMEM;
				insp = list->next;
				list = clone;
			} else {
				/* This may be pulled without problems. */
				insp = list;
			}
			if (pskb_carve(list, eat, gfp_mask) < 0) {
				kfree_skb(clone);
				return -ENOMEM;
			}
			break;
		}
	} while (eat);

	/* Free pulled out fragments. */
	while ((list = shinfo->frag_list) != insp) {
		shinfo->frag_list = list->next;
		kfree_skb(list);
	}
	/* And insert new clone at head. */
	if (clone) {
		clone->next = list;
		shinfo->frag_list = clone;
	}
	return 0;
}
