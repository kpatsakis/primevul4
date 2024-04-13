static inline void __dget_dlock(struct dentry *dentry)
{
	dentry->d_lockref.count++;
}
