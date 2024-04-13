static void __reg_combine_32_into_64(struct bpf_reg_state *reg)
{
	/* special case when 64-bit register has upper 32-bit register
	 * zeroed. Typically happens after zext or <<32, >>32 sequence
	 * allowing us to use 32-bit bounds directly,
	 */
	if (tnum_equals_const(tnum_clear_subreg(reg->var_off), 0)) {
		__reg_assign_32_into_64(reg);
	} else {
		/* Otherwise the best we can do is push lower 32bit known and
		 * unknown bits into register (var_off set from jmp logic)
		 * then learn as much as possible from the 64-bit tnum
		 * known and unknown bits. The previous smin/smax bounds are
		 * invalid here because of jmp32 compare so mark them unknown
		 * so they do not impact tnum bounds calculation.
		 */
		__mark_reg64_unbounded(reg);
		__update_reg_bounds(reg);
	}

	/* Intersecting with the old var_off might have improved our bounds
	 * slightly.  e.g. if umax was 0x7f...f and var_off was (0; 0xf...fc),
	 * then new var_off is (0; 0x7f...fc) which improves our umax.
	 */
	__reg_deduce_bounds(reg);
	__reg_bound_offset(reg);
	__update_reg_bounds(reg);
}