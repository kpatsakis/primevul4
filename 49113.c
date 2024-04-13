static void __exit des_s390_exit(void)
{
	if (ctrblk) {
		crypto_unregister_alg(&ctr_des_alg);
		crypto_unregister_alg(&ctr_des3_alg);
		free_page((unsigned long) ctrblk);
	}
	crypto_unregister_alg(&cbc_des3_alg);
	crypto_unregister_alg(&ecb_des3_alg);
	crypto_unregister_alg(&des3_alg);
	crypto_unregister_alg(&cbc_des_alg);
	crypto_unregister_alg(&ecb_des_alg);
	crypto_unregister_alg(&des_alg);
}
