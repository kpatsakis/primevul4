int crypto_register_template(struct crypto_template *tmpl)
{
	struct crypto_template *q;
	int err = -EEXIST;

	down_write(&crypto_alg_sem);

	crypto_check_module_sig(tmpl->module);

	list_for_each_entry(q, &crypto_template_list, list) {
		if (q == tmpl)
			goto out;
	}

	list_add(&tmpl->list, &crypto_template_list);
	crypto_notify(CRYPTO_MSG_TMPL_REGISTER, tmpl);
	err = 0;
out:
	up_write(&crypto_alg_sem);
	return err;
}
