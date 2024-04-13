int tcp_md5_hash_header(struct tcp_md5sig_pool *hp,
			struct tcphdr *th)
{
	struct scatterlist sg;
	int err;

	__sum16 old_checksum = th->check;
	th->check = 0;
	/* options aren't included in the hash */
	sg_init_one(&sg, th, sizeof(struct tcphdr));
	err = crypto_hash_update(&hp->md5_desc, &sg, sizeof(struct tcphdr));
	th->check = old_checksum;
	return err;
}
