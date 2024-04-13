static inline void allow_bits(struct posix_ace_state *astate, u32 mask)
{
	/* Allow all bits in the mask not already denied: */
	astate->allow |= mask & ~astate->deny;
}
