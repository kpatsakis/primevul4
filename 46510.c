static int sg_proc_open_dev(struct inode *inode, struct file *file)
{
        return seq_open(file, &dev_seq_ops);
}
