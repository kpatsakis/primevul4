static int bnep_rx_extension(struct bnep_session *s, struct sk_buff *skb)
{
	struct bnep_ext_hdr *h;
	int err = 0;

	do {
		h = (void *) skb->data;
		if (!skb_pull(skb, sizeof(*h))) {
			err = -EILSEQ;
			break;
		}

		BT_DBG("type 0x%x len %d", h->type, h->len);

		switch (h->type & BNEP_TYPE_MASK) {
		case BNEP_EXT_CONTROL:
			bnep_rx_control(s, skb->data, skb->len);
			break;

		default:
			/* Unknown extension, skip it. */
			break;
		}

		if (!skb_pull(skb, h->len)) {
			err = -EILSEQ;
			break;
		}
	} while (!err && (h->type & BNEP_EXT_HEADER));

	return err;
}
