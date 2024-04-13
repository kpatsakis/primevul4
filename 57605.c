static int skcipher_next_fast(struct skcipher_walk *walk)
{
	unsigned long diff;

	walk->src.phys.page = scatterwalk_page(&walk->in);
	walk->src.phys.offset = offset_in_page(walk->in.offset);
	walk->dst.phys.page = scatterwalk_page(&walk->out);
	walk->dst.phys.offset = offset_in_page(walk->out.offset);

	if (walk->flags & SKCIPHER_WALK_PHYS)
		return 0;

	diff = walk->src.phys.offset - walk->dst.phys.offset;
	diff |= walk->src.virt.page - walk->dst.virt.page;

	skcipher_map_src(walk);
	walk->dst.virt.addr = walk->src.virt.addr;

	if (diff) {
		walk->flags |= SKCIPHER_WALK_DIFF;
		skcipher_map_dst(walk);
	}

	return 0;
}
