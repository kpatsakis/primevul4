static inline void deny_bits(struct posix_ace_state *astate, u32 mask)
{
	/* Deny all bits in the mask not already allowed: */
	astate->deny |= mask & ~astate->allow;
}
