static inline void skcipher_unmap(struct scatter_walk *walk, void *vaddr)
{
	if (PageHighMem(scatterwalk_page(walk)))
		kunmap_atomic(vaddr);
}
