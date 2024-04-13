bool JSON_parser(Variant &z, const char *p, int length, bool const assoc,
                 int depth, int64_t options) {
  // No GC safepoints during JSON parsing, please. Code is not re-entrant.
  NoHandleSurpriseScope no_surprise(SafepointFlags);

  json_parser *json = s_json_parser.get(); /* the parser state */
  // Clear and reuse the thread-local string buffers. They are only freed if
  // they exceed kMaxPersistentStringBufferCapacity at exit or if the thread
  // is explicitly flushed (e.g., due to being idle).
  json->initSb(length);
  SCOPE_EXIT {
    constexpr int kMaxPersistentStringBufferCapacity = 256 * 1024;
    if (json->sb_cap > kMaxPersistentStringBufferCapacity) json->flushSb();
  };
  // SimpleParser only handles the most common set of options. Also, only use it
  // if its array nesting depth check is *more* restrictive than what the user
  // asks for, to ensure that the precise semantics of the general case is
  // applied for all nesting overflows.
  if (assoc &&
      options == (options & (k_JSON_FB_LOOSE |
                             k_JSON_FB_DARRAYS |
                             k_JSON_FB_DARRAYS_AND_VARRAYS |
                             k_JSON_FB_HACK_ARRAYS |
                             k_JSON_FB_THRIFT_SIMPLE_JSON |
                             k_JSON_FB_LEGACY_HACK_ARRAYS)) &&
      depth >= SimpleParser::kMaxArrayDepth &&
      length <= RuntimeOption::EvalSimpleJsonMaxLength &&
      SimpleParser::TryParse(p, length, json->tl_buffer.tv, z,
                             get_container_type_from_options(options),
                             options & k_JSON_FB_THRIFT_SIMPLE_JSON)) {
    return true;
  }

  int b;  /* the next character */
  int c;  /* the next character class */
  int s;  /* the next state */
  int state = 0;

  /*<fb>*/
  bool const loose = options & k_JSON_FB_LOOSE;
  JSONContainerType const container_type =
    get_container_type_from_options(options);
  int qchr = 0;
  int8_t const *byte_class;
  int8_t const (*next_state_table)[32];
  if (loose) {
    byte_class = loose_ascii_class;
    next_state_table = loose_state_transition_table;
  } else {
    byte_class = ascii_class;
    next_state_table = state_transition_table;
  }
  /*</fb>*/

  UncheckedBuffer *buf = &json->sb_buf;
  UncheckedBuffer *key = &json->sb_key;

  DataType type = kInvalidDataType;
  unsigned short escaped_bytes = 0;

  auto reset_type = [&] { type = kInvalidDataType; };

  json->depth = depth;
  // Since the stack is maintainined on a per request basis, for performance
  // reasons, it only makes sense to expand if necessary and cycles are wasted
  // contracting. Calls with a depth other than default should be rare.
  if (depth > json->stack.size()) {
    json->stack.resize(depth);
  }
  SCOPE_EXIT {
    if (json->stack.empty()) return;
    for (int i = 0; i <= json->mark; i++) {
      json->stack[i].key.reset();
      json->stack[i].val.unset();
    }
    json->mark = -1;
  };

  json->mark = json->top = -1;
  push(json, Mode::DONE);

  UTF8To16Decoder decoder(p, length, loose);
  for (;;) {
    b = decoder.decode();
    // Fast-case most common transition: append a simple string character.
    if (state == 3 && type == KindOfString) {
      while (b != '\"' &&  b != '\\' && b != '\'' && b <= 127 && b >= ' ') {
        buf->append((char)b);
        b = decoder.decode();
      }
    }
    if (b == UTF8_END) break; // UTF-8 decoding finishes successfully.
    if (b == UTF8_ERROR) {
      s_json_parser->error_code = JSON_ERROR_UTF8;
      return false;
    }
    assertx(b >= 0);

    if ((b & 127) == b) {
      /*<fb>*/
      c = byte_class[b];
      /*</fb>*/
      if (c <= S_ERR) {
        s_json_parser->error_code = JSON_ERROR_CTRL_CHAR;
        return false;
      }
    } else {
      c = S_ETC;
    }
    /*
      Get the next state from the transition table.
    */

    /*<fb>*/
    s = next_state_table[state][c];

    if (s == -4) {
      if (b != qchr) {
        s = 3;
      } else {
        qchr = 0;
      }
    }
    /*</fb>*/

    if (s < 0) {
      /*
        Perform one of the predefined actions.
      */
      switch (s) {
        /*
          empty }
        */
      case -9:
        /*<fb>*/
        if (json->top == 1) z = json->stack[json->top].val;
        else {
        /*</fb>*/
          attach_zval(json, json->stack[json->top].key, assoc, container_type);
        /*<fb>*/
        }
        /*</fb>*/
        if (!pop(json, Mode::KEY)) {
          return false;
        }
        state = 9;
        break;
        /*
          {
        */
      case -8:
        if (!push(json, Mode::KEY)) {
          s_json_parser->error_code = JSON_ERROR_DEPTH;
          return false;
        }

        state = 1;
        if (json->top > 0) {
          Variant &top = json->stack[json->top].val;
          /*<fb>*/
          if (container_type == JSONContainerType::COLLECTIONS) {
            // stable_maps is meaningless
            top = req::make<c_Map>();
          } else {
          /*</fb>*/
            if (!assoc) {
              top = SystemLib::AllocStdClassObject();
            /* <fb> */
            } else if (container_type == JSONContainerType::HACK_ARRAYS) {
              top = Array::CreateDict();
            } else if (container_type == JSONContainerType::DARRAYS ||
                       container_type == JSONContainerType::DARRAYS_AND_VARRAYS)
            {
              top = Array::CreateDArray();
            /* </fb> */
            } else if (
              container_type == JSONContainerType::LEGACY_HACK_ARRAYS) {
              auto arr = staticEmptyDictArray()->copy();
              arr->setLegacyArray(true);
              top = arr;
            } else {
              top = Array::CreateDArray();
            }
          /*<fb>*/
          }
          /*</fb>*/
          json->stack[json->top].key = copy_and_clear(*key);
          reset_type();
        }
        break;
        /*
          }
        */
      case -7:
        /*** BEGIN Facebook: json_utf8_loose ***/
        /*
          If this is a trailing comma in an object definition,
          we're in Mode::KEY. In that case, throw that off the
          stack and restore Mode::OBJECT so that we pretend the
          trailing comma just didn't happen.
        */
        if (loose) {
          if (pop(json, Mode::KEY)) {
            push(json, Mode::OBJECT);
          }
        }
        /*** END Facebook: json_utf8_loose ***/

        if (type != kInvalidDataType &&
            json->stack[json->top].mode == Mode::OBJECT) {
          Variant mval;
          json_create_zval(mval, *buf, type, options);
          Variant &top = json->stack[json->top].val;
          object_set(json, top, copy_and_clear(*key),
                     mval, assoc, container_type);
          buf->clear();
          reset_type();
        }

        /*<fb>*/
        if (json->top == 1) z = json->stack[json->top].val;
        else {
        /*</fb>*/
          attach_zval(json, json->stack[json->top].key,
            assoc, container_type);
        /*<fb>*/
        }
        /*</fb>*/
        if (!pop(json, Mode::OBJECT)) {
          s_json_parser->error_code = JSON_ERROR_STATE_MISMATCH;
          return false;
        }
        state = 9;
        break;
        /*
          [
        */
      case -6:
        if (!push(json, Mode::ARRAY)) {
          s_json_parser->error_code = JSON_ERROR_DEPTH;
          return false;
        }
        state = 2;

        if (json->top > 0) {
          Variant &top = json->stack[json->top].val;
          /*<fb>*/
          if (container_type == JSONContainerType::COLLECTIONS) {
            top = req::make<c_Vector>();
          } else if (container_type == JSONContainerType::HACK_ARRAYS) {
            top = Array::CreateVec();
          } else if (container_type == JSONContainerType::DARRAYS_AND_VARRAYS) {
            top = Array::CreateVArray();
          } else if (container_type == JSONContainerType::DARRAYS) {
            top = Array::CreateDArray();
          } else if (container_type == JSONContainerType::LEGACY_HACK_ARRAYS) {
            auto arr = staticEmptyVecArray()->copy();
            arr->setLegacyArray(true);
            top = arr;
          } else {
            top = Array::CreateDArray();
          }
          /*</fb>*/
          json->stack[json->top].key = copy_and_clear(*key);
          reset_type();
        }
        break;
        /*
          ]
        */
      case -5:
        {
          if (type != kInvalidDataType &&
               json->stack[json->top].mode == Mode::ARRAY) {
            Variant mval;
            json_create_zval(mval, *buf, type, options);
            auto& top = json->stack[json->top].val;
            if (container_type == JSONContainerType::COLLECTIONS) {
              collections::append(top.getObjectData(), mval.asTypedValue());
            } else {
              top.asArrRef().append(mval);
            }
            buf->clear();
            reset_type();
          }

          /*<fb>*/
          if (json->top == 1) z = json->stack[json->top].val;
          else {
          /*</fb>*/
            attach_zval(json, json->stack[json->top].key, assoc,
              container_type);
          /*<fb>*/
          }
          /*</fb>*/
          if (!pop(json, Mode::ARRAY)) {
            s_json_parser->error_code = JSON_ERROR_STATE_MISMATCH;
            return false;
          }
          state = 9;
        }
        break;
        /*
          "
        */
      case -4:
        switch (json->stack[json->top].mode) {
        case Mode::KEY:
          state = 27;
          std::swap(buf, key);
          reset_type();
          break;
        case Mode::ARRAY:
        case Mode::OBJECT:
          state = 9;
          break;
        case Mode::DONE:
          if (type == KindOfString) {
            z = copy_and_clear(*buf);
            state = 9;
            break;
          }
          /* fall through if not KindOfString */
        default:
          s_json_parser->error_code = JSON_ERROR_SYNTAX;
          return false;
        }
        break;
        /*
          ,
        */
      case -3:
        {
          Variant mval;
          if (type != kInvalidDataType &&
              (json->stack[json->top].mode == Mode::OBJECT ||
               json->stack[json->top].mode == Mode::ARRAY)) {
            json_create_zval(mval, *buf, type, options);
          }

          switch (json->stack[json->top].mode) {
          case Mode::OBJECT:
            if (pop(json, Mode::OBJECT) &&
                push(json, Mode::KEY)) {
              if (type != kInvalidDataType) {
                Variant &top = json->stack[json->top].val;
                object_set(
                  json,
                  top,
                  copy_and_clear(*key),
                  mval,
                  assoc,
                  container_type
                );
              }
              state = 29;
            }
            break;
          case Mode::ARRAY:
            if (type != kInvalidDataType) {
              auto& top = json->stack[json->top].val;
              if (container_type == JSONContainerType::COLLECTIONS) {
                collections::append(top.getObjectData(), mval.asTypedValue());
              } else {
                top.asArrRef().append(mval);
              }
            }
            state = 28;
            break;
          default:
            s_json_parser->error_code = JSON_ERROR_SYNTAX;
            return false;
          }
          buf->clear();
          reset_type();
          check_non_safepoint_surprise();
        }
        break;

        /*<fb>*/
        /*
          : (after unquoted string)
        */
      case -10:
        if (json->stack[json->top].mode == Mode::KEY) {
          state = 27;
          std::swap(buf, key);
          reset_type();
          s = -2;
        } else {
          s = 3;
          break;
        }
        /*</fb>*/

        /*
          :
        */
      case -2:
        if (pop(json, Mode::KEY) && push(json, Mode::OBJECT)) {
          state = 28;
          break;
        }
        /*
          syntax error
        */
      case -1:
        s_json_parser->error_code = JSON_ERROR_SYNTAX;
        return false;
      }
    } else {
      /*
        Change the state and iterate.
      */
      bool is_tsimplejson = options & k_JSON_FB_THRIFT_SIMPLE_JSON;
      if (type == KindOfString) {
        if (/*<fb>*/(/*</fb>*/s == 3/*<fb>*/ || s == 30)/*</fb>*/ &&
            state != 8) {
          if (state != 4) {
            utf16_to_utf8(*buf, b);
          } else {
            switch (b) {
            case 'b': buf->append('\b'); break;
            case 't': buf->append('\t'); break;
            case 'n': buf->append('\n'); break;
            case 'f': buf->append('\f'); break;
            case 'r': buf->append('\r'); break;
            default:
              utf16_to_utf8(*buf, b);
              break;
            }
          }
        } else if (s == 6) {
          if (UNLIKELY(is_tsimplejson)) {
            if (UNLIKELY(b != '0'))  {
              s_json_parser->error_code = JSON_ERROR_SYNTAX;
              return false;
            }
            escaped_bytes = 0;
          } else {
            escaped_bytes = dehexchar(b) << 12;
          }
        } else if (s == 7) {
          if (UNLIKELY(is_tsimplejson)) {
            if (UNLIKELY(b != '0'))  {
              s_json_parser->error_code = JSON_ERROR_SYNTAX;
              return false;
            }
          } else {
            escaped_bytes += dehexchar(b) << 8;
          }
        } else if (s == 8) {
          escaped_bytes += dehexchar(b) << 4;
        } else if (s == 3 && state == 8) {
          escaped_bytes += dehexchar(b);
          if (UNLIKELY(is_tsimplejson)) {
            buf->append((char)escaped_bytes);
          } else {
            utf16_to_utf8(*buf, escaped_bytes);
          }
        }
      } else if ((type == kInvalidDataType || type == KindOfNull) &&
                 (c == S_DIG || c == S_ZER)) {
        type = KindOfInt64;
        buf->append((char)b);
      } else if (type == KindOfInt64 && s == 24) {
        type = KindOfDouble;
        buf->append((char)b);
      } else if ((type == kInvalidDataType || type == KindOfNull ||
                  type == KindOfInt64) &&
                 c == S_DOT) {
        type = KindOfDouble;
        buf->append((char)b);
      } else if (type != KindOfString && c == S_QUO) {
        type = KindOfString;
        /*<fb>*/qchr = b;/*</fb>*/
      } else if ((type == kInvalidDataType || type == KindOfNull ||
                  type == KindOfInt64 || type == KindOfDouble) &&
                 ((state == 12 && s == 9) ||
                  (state == 16 && s == 9))) {
        type = KindOfBoolean;
      } else if (type == kInvalidDataType && state == 19 && s == 9) {
        type = KindOfNull;
      } else if (type != KindOfString && c > S_WSP) {
        utf16_to_utf8(*buf, b);
      }

      state = s;
    }
  }

  if (state == 9 && pop(json, Mode::DONE)) {
    s_json_parser->error_code = JSON_ERROR_NONE;
    return true;
  }

  s_json_parser->error_code = JSON_ERROR_SYNTAX;
  return false;
}