options_from_list(w, priv)
    struct wordlist *w;
    int priv;
{
    char *argv[MAXARGS];
    option_t *opt;
    int i, n, ret = 0;
    struct wordlist *w0;

    privileged_option = priv;
    option_source = "secrets file";
    option_priority = OPRIO_SECFILE;

    while (w != NULL) {
	opt = find_option(w->word);
	if (opt == NULL) {
	    option_error("In secrets file: unrecognized option '%s'",
			 w->word);
	    goto err;
	}
	n = n_arguments(opt);
	w0 = w;
	for (i = 0; i < n; ++i) {
	    w = w->next;
	    if (w == NULL) {
		option_error(
			"In secrets file: too few parameters for option '%s'",
			w0->word);
		goto err;
	    }
	    argv[i] = w->word;
	}
	if (!process_option(opt, w0->word, argv))
	    goto err;
	w = w->next;
    }
    ret = 1;

err:
    return ret;
}
