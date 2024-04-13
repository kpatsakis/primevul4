int ssl3_send_client_verify(SSL *s)
	{
	unsigned char *p;
	unsigned char data[MD5_DIGEST_LENGTH+SHA_DIGEST_LENGTH];
	EVP_PKEY *pkey;
	EVP_PKEY_CTX *pctx=NULL;
	EVP_MD_CTX mctx;
	unsigned u=0;
	unsigned long n;
	int j;

	EVP_MD_CTX_init(&mctx);

	if (s->state == SSL3_ST_CW_CERT_VRFY_A)
		{
		p= ssl_handshake_start(s);
		pkey=s->cert->key->privatekey;
/* Create context from key and test if sha1 is allowed as digest */
		pctx = EVP_PKEY_CTX_new(pkey,NULL);
		EVP_PKEY_sign_init(pctx);
		if (EVP_PKEY_CTX_set_signature_md(pctx, EVP_sha1())>0)
			{
			if (!SSL_USE_SIGALGS(s))
				s->method->ssl3_enc->cert_verify_mac(s,
						NID_sha1,
						&(data[MD5_DIGEST_LENGTH]));
			}
		else
			{
			ERR_clear_error();
			}
		/* For TLS v1.2 send signature algorithm and signature
		 * using agreed digest and cached handshake records.
		 */
		if (SSL_USE_SIGALGS(s))
			{
			long hdatalen = 0;
			void *hdata;
			const EVP_MD *md = s->cert->key->digest;
			hdatalen = BIO_get_mem_data(s->s3->handshake_buffer,
								&hdata);
			if (hdatalen <= 0 || !tls12_get_sigandhash(p, pkey, md))
				{
				SSLerr(SSL_F_SSL3_SEND_CLIENT_VERIFY,
						ERR_R_INTERNAL_ERROR);
				goto err;
				}
			p += 2;
#ifdef SSL_DEBUG
			fprintf(stderr, "Using TLS 1.2 with client alg %s\n",
							EVP_MD_name(md));
#endif
			if (!EVP_SignInit_ex(&mctx, md, NULL)
				|| !EVP_SignUpdate(&mctx, hdata, hdatalen)
				|| !EVP_SignFinal(&mctx, p + 2, &u, pkey))
				{
				SSLerr(SSL_F_SSL3_SEND_CLIENT_VERIFY,
						ERR_R_EVP_LIB);
				goto err;
				}
			s2n(u,p);
			n = u + 4;
			if (!ssl3_digest_cached_records(s))
				goto err;
			}
		else
#ifndef OPENSSL_NO_RSA
		if (pkey->type == EVP_PKEY_RSA)
			{
			s->method->ssl3_enc->cert_verify_mac(s,
				NID_md5,
			 	&(data[0]));
			if (RSA_sign(NID_md5_sha1, data,
					 MD5_DIGEST_LENGTH+SHA_DIGEST_LENGTH,
					&(p[2]), &u, pkey->pkey.rsa) <= 0 )
				{
				SSLerr(SSL_F_SSL3_SEND_CLIENT_VERIFY,ERR_R_RSA_LIB);
				goto err;
				}
			s2n(u,p);
			n=u+2;
			}
		else
#endif
#ifndef OPENSSL_NO_DSA
			if (pkey->type == EVP_PKEY_DSA)
			{
			if (!DSA_sign(pkey->save_type,
				&(data[MD5_DIGEST_LENGTH]),
				SHA_DIGEST_LENGTH,&(p[2]),
				(unsigned int *)&j,pkey->pkey.dsa))
				{
				SSLerr(SSL_F_SSL3_SEND_CLIENT_VERIFY,ERR_R_DSA_LIB);
				goto err;
				}
			s2n(j,p);
			n=j+2;
			}
		else
#endif
#ifndef OPENSSL_NO_ECDSA
			if (pkey->type == EVP_PKEY_EC)
			{
			if (!ECDSA_sign(pkey->save_type,
				&(data[MD5_DIGEST_LENGTH]),
				SHA_DIGEST_LENGTH,&(p[2]),
				(unsigned int *)&j,pkey->pkey.ec))
				{
				SSLerr(SSL_F_SSL3_SEND_CLIENT_VERIFY,
				    ERR_R_ECDSA_LIB);
				goto err;
				}
			s2n(j,p);
			n=j+2;
			}
		else
#endif
		if (pkey->type == NID_id_GostR3410_94 || pkey->type == NID_id_GostR3410_2001) 
		{
		unsigned char signbuf[64];
		int i;
		size_t sigsize=64;
		s->method->ssl3_enc->cert_verify_mac(s,
			NID_id_GostR3411_94,
			data);
		if (EVP_PKEY_sign(pctx, signbuf, &sigsize, data, 32) <= 0) {
			SSLerr(SSL_F_SSL3_SEND_CLIENT_VERIFY,
			ERR_R_INTERNAL_ERROR);
			goto err;
		}
		for (i=63,j=0; i>=0; j++, i--) {
			p[2+j]=signbuf[i];
		}	
		s2n(j,p);
		n=j+2;
		}
		else
		{
			SSLerr(SSL_F_SSL3_SEND_CLIENT_VERIFY,ERR_R_INTERNAL_ERROR);
			goto err;
		}
		ssl_set_handshake_header(s, SSL3_MT_CERTIFICATE_VERIFY, n);
		s->state=SSL3_ST_CW_CERT_VRFY_B;
		}
	EVP_MD_CTX_cleanup(&mctx);
	EVP_PKEY_CTX_free(pctx);
	return ssl_do_write(s);
err:
	EVP_MD_CTX_cleanup(&mctx);
	EVP_PKEY_CTX_free(pctx);
	return(-1);
	}
