static inline void pfkey_hdr_dup(struct sadb_msg *new,
				 const struct sadb_msg *orig)
{
	*new = *orig;
}
