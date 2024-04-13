char *simple_dname(struct dentry *dentry, char *buffer, int buflen)
{
	char *end = buffer + buflen;
	/* these dentries are never renamed, so d_lock is not needed */
	if (prepend(&end, &buflen, " (deleted)", 11) ||
	    prepend(&end, &buflen, dentry->d_name.name, dentry->d_name.len) ||
	    prepend(&end, &buflen, "/", 1))  
		end = ERR_PTR(-ENAMETOOLONG);
	return end;
}
