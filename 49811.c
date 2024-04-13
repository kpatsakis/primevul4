static int ablk_crypt(struct ablkcipher_request *areq)
{
	struct ablkcipher_walk walk;
	struct crypto_ablkcipher *cipher = crypto_ablkcipher_reqtfm(areq);
	struct cryp_ctx *ctx = crypto_ablkcipher_ctx(cipher);
	struct cryp_device_data *device_data;
	unsigned long src_paddr;
	unsigned long dst_paddr;
	int ret;
	int nbytes;

	pr_debug(DEV_DBG_NAME " [%s]", __func__);

	ret = cryp_get_device_data(ctx, &device_data);
	if (ret)
		goto out;

	ablkcipher_walk_init(&walk, areq->dst, areq->src, areq->nbytes);
	ret = ablkcipher_walk_phys(areq, &walk);

	if (ret) {
		pr_err(DEV_DBG_NAME "[%s]: ablkcipher_walk_phys() failed!",
			__func__);
		goto out;
	}

	while ((nbytes = walk.nbytes) > 0) {
		ctx->iv = walk.iv;
		src_paddr = (page_to_phys(walk.src.page) + walk.src.offset);
		ctx->indata = phys_to_virt(src_paddr);

		dst_paddr = (page_to_phys(walk.dst.page) + walk.dst.offset);
		ctx->outdata = phys_to_virt(dst_paddr);

		ctx->datalen = nbytes - (nbytes % ctx->blocksize);

		ret = hw_crypt_noxts(ctx, device_data);
		if (ret)
			goto out;

		nbytes -= ctx->datalen;
		ret = ablkcipher_walk_done(areq, &walk, nbytes);
		if (ret)
			goto out;
	}
	ablkcipher_walk_complete(&walk);

out:
	/* Release the device */
	spin_lock(&device_data->ctx_lock);
	device_data->current_ctx = NULL;
	ctx->device = NULL;
	spin_unlock(&device_data->ctx_lock);

	/*
	 * The down_interruptible part for this semaphore is called in
	 * cryp_get_device_data.
	 */
	up(&driver_data.device_allocation);

	return ret;
}
