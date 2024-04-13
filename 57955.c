int mailimf_msg_id_list_parse(const char * message, size_t length,
			      size_t * indx, clist ** result)
{
  return mailimf_struct_multiple_parse(message, length, indx,
				       result,
				       (mailimf_struct_parser *)
				       mailimf_unstrict_msg_id_parse,
				       (mailimf_struct_destructor *)
				       mailimf_msg_id_free);
}
