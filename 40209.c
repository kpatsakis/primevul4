void __ptrace_link(struct task_struct *child, struct task_struct *new_parent)
{
	BUG_ON(!list_empty(&child->ptrace_entry));
	list_add(&child->ptrace_entry, &new_parent->ptraced);
	child->parent = new_parent;
}
