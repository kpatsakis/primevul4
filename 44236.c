static ssize_t cm_show_counter(struct kobject *obj, struct attribute *attr,
			       char *buf)
{
	struct cm_counter_group *group;
	struct cm_counter_attribute *cm_attr;

	group = container_of(obj, struct cm_counter_group, obj);
	cm_attr = container_of(attr, struct cm_counter_attribute, attr);

	return sprintf(buf, "%ld\n",
		       atomic_long_read(&group->counter[cm_attr->index]));
}
