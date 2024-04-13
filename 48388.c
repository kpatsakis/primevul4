void cryptd_free_ahash(struct cryptd_ahash *tfm)
{
	crypto_free_ahash(&tfm->base);
}
