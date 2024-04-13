GF_Err gppc_box_size(GF_Box *s)
{
	GF_3GPPConfigBox *ptr = (GF_3GPPConfigBox *)s;

	s->size += 5;
	if (!ptr->cfg.type) {
		switch (ptr->type) {
		case GF_ISOM_BOX_TYPE_D263:
			ptr->cfg.type = GF_ISOM_SUBTYPE_3GP_H263;
			break;
		case GF_ISOM_BOX_TYPE_DAMR:
			ptr->cfg.type = GF_ISOM_SUBTYPE_3GP_AMR;
			break;
		case GF_ISOM_BOX_TYPE_DEVC:
			ptr->cfg.type = GF_ISOM_SUBTYPE_3GP_EVRC;
			break;
		case GF_ISOM_BOX_TYPE_DQCP:
			ptr->cfg.type = GF_ISOM_SUBTYPE_3GP_QCELP;
			break;
		case GF_ISOM_BOX_TYPE_DSMV:
			ptr->cfg.type = GF_ISOM_SUBTYPE_3GP_SMV;
			break;
		}
	}
	switch (ptr->cfg.type) {
	case GF_ISOM_SUBTYPE_3GP_H263:
		s->size += 2;
		break;
	case GF_ISOM_SUBTYPE_3GP_AMR:
	case GF_ISOM_SUBTYPE_3GP_AMR_WB:
		s->size += 4;
		break;
	case GF_ISOM_SUBTYPE_3GP_EVRC:
	case GF_ISOM_SUBTYPE_3GP_QCELP:
	case GF_ISOM_SUBTYPE_3GP_SMV:
		s->size += 1;
		break;
	}
	return GF_OK;
}