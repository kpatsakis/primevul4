void __put_anon_vma(struct anon_vma *anon_vma)
{
	struct anon_vma *root = anon_vma->root;

	if (root != anon_vma && atomic_dec_and_test(&root->refcount))
		anon_vma_free(root);

	anon_vma_free(anon_vma);
}
