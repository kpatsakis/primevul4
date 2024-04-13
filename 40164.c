int bprm_change_interp(char *interp, struct linux_binprm *bprm)
{
	/* If a binfmt changed the interp, free it first. */
	if (bprm->interp != bprm->filename)
		kfree(bprm->interp);
	bprm->interp = kstrdup(interp, GFP_KERNEL);
	if (!bprm->interp)
		return -ENOMEM;
	return 0;
}
