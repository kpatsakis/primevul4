static inline int ablkcipher_next_fast(struct ablkcipher_request *req,
				       struct ablkcipher_walk *walk)
{
	walk->src.page = scatterwalk_page(&walk->in);
	walk->src.offset = offset_in_page(walk->in.offset);
	walk->dst.page = scatterwalk_page(&walk->out);
	walk->dst.offset = offset_in_page(walk->out.offset);

	return 0;
}
