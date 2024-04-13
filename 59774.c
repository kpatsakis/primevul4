static int nfs_idmap_prepare_message(char *desc, struct idmap *idmap,
				     struct idmap_msg *im,
				     struct rpc_pipe_msg *msg)
{
	substring_t substr;
	int token, ret;

	im->im_type = IDMAP_TYPE_GROUP;
	token = match_token(desc, nfs_idmap_tokens, &substr);

	switch (token) {
	case Opt_find_uid:
		im->im_type = IDMAP_TYPE_USER;
	case Opt_find_gid:
		im->im_conv = IDMAP_CONV_NAMETOID;
		ret = match_strlcpy(im->im_name, &substr, IDMAP_NAMESZ);
		break;

	case Opt_find_user:
		im->im_type = IDMAP_TYPE_USER;
	case Opt_find_group:
		im->im_conv = IDMAP_CONV_IDTONAME;
		ret = match_int(&substr, &im->im_id);
		break;

	default:
		ret = -EINVAL;
		goto out;
	}

	msg->data = im;
	msg->len  = sizeof(struct idmap_msg);

out:
	return ret;
}
