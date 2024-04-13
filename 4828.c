skip_attr_bytes (unsigned long form,
		 unsigned char *data,
		 unsigned char *end,
		 dwarf_vma pointer_size,
		 dwarf_vma offset_size,
		 int dwarf_version,
		 dwarf_vma *value_return)
{
  dwarf_signed_vma svalue;
  dwarf_vma uvalue = 0;
  dwarf_vma inc = 0;

  * value_return = 0;

  switch (form)
    {
    case DW_FORM_ref_addr:
      if (dwarf_version == 2)
	SAFE_BYTE_GET_AND_INC (uvalue, data, pointer_size, end);
      else if (dwarf_version > 2)
	SAFE_BYTE_GET_AND_INC (uvalue, data, offset_size, end);
      else
	return NULL;
      break;

    case DW_FORM_addr:
      SAFE_BYTE_GET_AND_INC (uvalue, data, pointer_size, end);
      break;

    case DW_FORM_strp:
    case DW_FORM_line_strp:
    case DW_FORM_sec_offset:
    case DW_FORM_GNU_ref_alt:
    case DW_FORM_GNU_strp_alt:
      SAFE_BYTE_GET_AND_INC (uvalue, data, offset_size, end);
      break;

    case DW_FORM_flag_present:
      uvalue = 1;
      break;

    case DW_FORM_ref1:
    case DW_FORM_flag:
    case DW_FORM_data1:
    case DW_FORM_strx1:
    case DW_FORM_addrx1:
      SAFE_BYTE_GET_AND_INC (uvalue, data, 1, end);
      break;

    case DW_FORM_strx3:
    case DW_FORM_addrx3:
      SAFE_BYTE_GET_AND_INC (uvalue, data, 3, end);
      break;

    case DW_FORM_ref2:
    case DW_FORM_data2:
    case DW_FORM_strx2:
    case DW_FORM_addrx2:
      SAFE_BYTE_GET_AND_INC (uvalue, data, 2, end);
      break;

    case DW_FORM_ref4:
    case DW_FORM_data4:
    case DW_FORM_strx4:
    case DW_FORM_addrx4:
      SAFE_BYTE_GET_AND_INC (uvalue, data, 4, end);
      break;

    case DW_FORM_sdata:
      READ_SLEB (svalue, data, end);
      uvalue = svalue;
      break;

    case DW_FORM_ref_udata:
    case DW_FORM_udata:
    case DW_FORM_GNU_str_index:
    case DW_FORM_strx:
    case DW_FORM_GNU_addr_index:
    case DW_FORM_addrx:
    case DW_FORM_loclistx:
    case DW_FORM_rnglistx:
      READ_ULEB (uvalue, data, end);
      break;

    case DW_FORM_ref8:
      SAFE_BYTE_GET_AND_INC (uvalue, data, 8, end);
      break;

    case DW_FORM_data8:
    case DW_FORM_ref_sig8:
      inc = 8;
      break;

    case DW_FORM_data16:
      inc = 16;
      break;

    case DW_FORM_string:
      inc = strnlen ((char *) data, end - data) + 1;
      break;

    case DW_FORM_block:
    case DW_FORM_exprloc:
      READ_ULEB (uvalue, data, end);
      inc = uvalue;
      break;

    case DW_FORM_block1:
      SAFE_BYTE_GET_AND_INC (uvalue, data, 1, end);
      inc = uvalue;
      break;

    case DW_FORM_block2:
      SAFE_BYTE_GET_AND_INC (uvalue, data, 2, end);
      inc = uvalue;
      break;

    case DW_FORM_block4:
      SAFE_BYTE_GET_AND_INC (uvalue, data, 4, end);
      inc = uvalue;
      break;

    case DW_FORM_indirect:
      READ_ULEB (form, data, end);
      if (form == DW_FORM_implicit_const)
	SKIP_ULEB (data, end);
      return skip_attr_bytes (form, data, end, pointer_size, offset_size,
			      dwarf_version, value_return);

    default:
      return NULL;
    }

  * value_return = uvalue;
  if (inc <= (dwarf_vma) (end - data))
    data += inc;
  else
    data = end;
  return data;
}