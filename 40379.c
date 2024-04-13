static inline unsigned int ablkcipher_done_slow(struct ablkcipher_walk *walk,
						unsigned int bsize)
{
	unsigned int n = bsize;

	for (;;) {
		unsigned int len_this_page = scatterwalk_pagelen(&walk->out);

		if (len_this_page > n)
			len_this_page = n;
		scatterwalk_advance(&walk->out, n);
		if (n == len_this_page)
			break;
		n -= len_this_page;
		scatterwalk_start(&walk->out, scatterwalk_sg_next(walk->out.sg));
	}

	return bsize;
}
