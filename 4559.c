
void gf_isom_box_remove_from_parent(GF_Box *parent_box, GF_Box *box)
{
	if (parent_box && parent_box->registry && parent_box->registry->add_rem_fn) {
		parent_box->registry->add_rem_fn(parent_box, box, GF_TRUE);
	}