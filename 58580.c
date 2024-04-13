static inline void __dget(struct dentry *dentry)
{
	lockref_get(&dentry->d_lockref);
}
