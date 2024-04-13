static void __bnep_copy_ci(struct bnep_conninfo *ci, struct bnep_session *s)
{
	memset(ci, 0, sizeof(*ci));
	memcpy(ci->dst, s->eh.h_source, ETH_ALEN);
	strcpy(ci->device, s->dev->name);
	ci->flags = s->flags;
	ci->state = s->state;
	ci->role  = s->role;
}
