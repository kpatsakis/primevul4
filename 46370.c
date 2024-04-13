monitor_apply_keystate(struct monitor *pmonitor)
{
	struct ssh *ssh = active_state;	/* XXX */
	struct kex *kex;
	int r;

	debug3("%s: packet_set_state", __func__);
	if ((r = ssh_packet_set_state(ssh, child_state)) != 0)
                fatal("%s: packet_set_state: %s", __func__, ssh_err(r));
	sshbuf_free(child_state);
	child_state = NULL;

	if ((kex = ssh->kex) != 0) {
		/* XXX set callbacks */
#ifdef WITH_OPENSSL
		kex->kex[KEX_DH_GRP1_SHA1] = kexdh_server;
		kex->kex[KEX_DH_GRP14_SHA1] = kexdh_server;
		kex->kex[KEX_DH_GEX_SHA1] = kexgex_server;
		kex->kex[KEX_DH_GEX_SHA256] = kexgex_server;
# ifdef OPENSSL_HAS_ECC
		kex->kex[KEX_ECDH_SHA2] = kexecdh_server;
# endif
#endif /* WITH_OPENSSL */
		kex->kex[KEX_C25519_SHA256] = kexc25519_server;
		kex->load_host_public_key=&get_hostkey_public_by_type;
		kex->load_host_private_key=&get_hostkey_private_by_type;
		kex->host_key_index=&get_hostkey_index;
		kex->sign = sshd_hostkey_sign;
	}

	/* Update with new address */
	if (options.compression) {
		ssh_packet_set_compress_hooks(ssh, pmonitor->m_zlib,
		    (ssh_packet_comp_alloc_func *)mm_zalloc,
		    (ssh_packet_comp_free_func *)mm_zfree);
	}
}
