kex_send_ext_info(struct ssh *ssh)
{
	int r;
	char *algs;

	if ((algs = sshkey_alg_list(0, 1, ',')) == NULL)
		return SSH_ERR_ALLOC_FAIL;
	if ((r = sshpkt_start(ssh, SSH2_MSG_EXT_INFO)) != 0 ||
	    (r = sshpkt_put_u32(ssh, 1)) != 0 ||
	    (r = sshpkt_put_cstring(ssh, "server-sig-algs")) != 0 ||
	    (r = sshpkt_put_cstring(ssh, algs)) != 0 ||
	    (r = sshpkt_send(ssh)) != 0)
		goto out;
	/* success */
	r = 0;
 out:
	free(algs);
	return r;
}
