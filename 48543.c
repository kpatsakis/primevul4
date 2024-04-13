static inline struct mcryptd_queue *mcryptd_get_queue(struct crypto_tfm *tfm)
{
	struct crypto_instance *inst = crypto_tfm_alg_instance(tfm);
	struct mcryptd_instance_ctx *ictx = crypto_instance_ctx(inst);

	return ictx->queue;
}
