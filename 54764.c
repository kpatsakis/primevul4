static void free_sglist(struct scatterlist *sg, int nents)
{
	unsigned		i;

	if (!sg)
		return;
	for (i = 0; i < nents; i++) {
		if (!sg_page(&sg[i]))
			continue;
		kfree(sg_virt(&sg[i]));
	}
	kfree(sg);
}
