void mcryptd_free_ahash(struct mcryptd_ahash *tfm)
{
	crypto_free_ahash(&tfm->base);
}
