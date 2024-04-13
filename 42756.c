static int irias_seq_show(struct seq_file *seq, void *v)
{
	if (v == SEQ_START_TOKEN)
		seq_puts(seq, "LM-IAS Objects:\n");
	else {
		struct ias_object *obj = v;
		struct ias_attrib *attrib;

		IRDA_ASSERT(obj->magic == IAS_OBJECT_MAGIC, return -EINVAL;);

		seq_printf(seq, "name: %s, id=%d\n",
			   obj->name, obj->id);

		/* Careful for priority inversions here !
		 * All other uses of attrib spinlock are independent of
		 * the object spinlock, so we are safe. Jean II */
		spin_lock(&obj->attribs->hb_spinlock);

		/* List all attributes for this object */
		for (attrib = (struct ias_attrib *) hashbin_get_first(obj->attribs);
		     attrib != NULL;
		     attrib = (struct ias_attrib *) hashbin_get_next(obj->attribs)) {

			IRDA_ASSERT(attrib->magic == IAS_ATTRIB_MAGIC,
				    goto outloop; );

			seq_printf(seq, " - Attribute name: \"%s\", ",
				   attrib->name);
			seq_printf(seq, "value[%s]: ",
				   ias_value_types[attrib->value->type]);

			switch (attrib->value->type) {
			case IAS_INTEGER:
				seq_printf(seq, "%d\n",
					   attrib->value->t.integer);
				break;
			case IAS_STRING:
				seq_printf(seq, "\"%s\"\n",
					   attrib->value->t.string);
				break;
			case IAS_OCT_SEQ:
				seq_printf(seq, "octet sequence (%d bytes)\n",
					   attrib->value->len);
				break;
			case IAS_MISSING:
				seq_puts(seq, "missing\n");
				break;
			default:
				seq_printf(seq, "type %d?\n",
					   attrib->value->type);
			}
			seq_putc(seq, '\n');

		}
	IRDA_ASSERT_LABEL(outloop:)
		spin_unlock(&obj->attribs->hb_spinlock);
	}

	return 0;
}
