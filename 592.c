bool WddxPacket::recursiveAddVar(const String& varName,
                                 const Variant& varVariant,
                                 bool hasVarTag) {

  bool isArray = varVariant.isArray();
  bool isObject = varVariant.isObject();

  if (isArray || isObject) {
    if (hasVarTag) {
      m_packetString.append("<var name='");
      m_packetString.append(varName.data());
      m_packetString.append("'>");
    }

    Array varAsArray;
    Object varAsObject = varVariant.toObject();
    if (isArray) varAsArray = varVariant.toArray();
    if (isObject) varAsArray = varAsObject.toArray();

    int length = varAsArray.length();
    if (length > 0) {
      ArrayIter it = ArrayIter(varAsArray);
      if (it.first().isString()) isObject = true;
      if (isObject) {
        m_packetString.append("<struct>");
        if (!isArray) {
          m_packetString.append("<var name='php_class_name'><string>");
          m_packetString.append(varAsObject->getClassName());
          m_packetString.append("</string></var>");
        }
      } else {
        m_packetString.append("<array length='");
        m_packetString.append(std::to_string(length));
        m_packetString.append("'>");
      }
      for (ArrayIter it(varAsArray); it; ++it) {
        Variant key = it.first();
        Variant value = it.second();
        recursiveAddVar(key.toString(), value, isObject);
      }
      if (isObject) {
        m_packetString.append("</struct>");
      }
      else {
        m_packetString.append("</array>");
      }
    }
    else {
      //empty object
      if (isObject) {
        m_packetString.append("<struct>");
        if (!isArray) {
          m_packetString.append("<var name='php_class_name'><string>");
          m_packetString.append(varAsObject->getClassName());
          m_packetString.append("</string></var>");
        }
        m_packetString.append("</struct>");
      }
    }
    if (hasVarTag) {
      m_packetString.append("</var>");
    }
    return true;
  }

  String varType = getDataTypeString(varVariant.getType());
  if (!getWddxEncoded(varType, "", varName, false).empty()) {
    String varValue;
    if (varType.compare("boolean") == 0) {
      varValue = varVariant.toBoolean() ? "true" : "false";
    } else {
      varValue = StringUtil::HtmlEncode(varVariant.toString(),
                                        StringUtil::QuoteStyle::Double,
                                        "UTF-8", false, false).toCppString();
    }
    m_packetString.append(
      getWddxEncoded(varType, varValue, varName, hasVarTag));
    return true;
  }

  return false;
}