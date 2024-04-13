static struct sadb_msg *pfkey_get_base_msg(struct sk_buff *skb, int *errp)
{
	struct sadb_msg *hdr = NULL;

	if (skb->len < sizeof(*hdr)) {
		*errp = -EMSGSIZE;
	} else {
		hdr = (struct sadb_msg *) skb->data;
		if (hdr->sadb_msg_version != PF_KEY_V2 ||
		    hdr->sadb_msg_reserved != 0 ||
		    (hdr->sadb_msg_type <= SADB_RESERVED ||
		     hdr->sadb_msg_type > SADB_MAX)) {
			hdr = NULL;
			*errp = -EINVAL;
		} else if (hdr->sadb_msg_len != (skb->len /
						 sizeof(uint64_t)) ||
			   hdr->sadb_msg_len < (sizeof(struct sadb_msg) /
						sizeof(uint64_t))) {
			hdr = NULL;
			*errp = -EMSGSIZE;
		} else {
			*errp = 0;
		}
	}
	return hdr;
}
