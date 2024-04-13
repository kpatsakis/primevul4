gsf_infile_tar_class_init (GObjectClass *gobject_class)
{
	GsfInputClass  *input_class  = GSF_INPUT_CLASS (gobject_class);
	GsfInfileClass *infile_class = GSF_INFILE_CLASS (gobject_class);

	gobject_class->constructor      = gsf_infile_tar_constructor;
	gobject_class->finalize		= gsf_infile_tar_finalize;
	gobject_class->dispose		= gsf_infile_tar_dispose;
	gobject_class->get_property     = gsf_infile_tar_get_property;
	gobject_class->set_property     = gsf_infile_tar_set_property;

	input_class->Dup		= gsf_infile_tar_dup;
	input_class->Read		= gsf_infile_tar_read;
	input_class->Seek		= gsf_infile_tar_seek;
	infile_class->num_children	= gsf_infile_tar_num_children;
	infile_class->name_by_index	= gsf_infile_tar_name_by_index;
	infile_class->child_by_index	= gsf_infile_tar_child_by_index;
	infile_class->child_by_name	= gsf_infile_tar_child_by_name;

	parent_class = g_type_class_peek_parent (gobject_class);

	g_object_class_install_property
		(gobject_class,
		 PROP_SOURCE,
		 g_param_spec_object ("source",
				      _("Source"),
				      _("The archive being interpreted"),
				      GSF_INPUT_TYPE,
				      GSF_PARAM_STATIC |
				      G_PARAM_READWRITE |
				      G_PARAM_CONSTRUCT_ONLY));
}
