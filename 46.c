  virtual std::string GetHtmlInfo(int refresh) {
    std::string output;
    output.append("<html><head><title>About Network</title>");
    if (refresh > 0)
      output.append("<meta http-equiv=\"refresh\" content=\"" +
          base::IntToString(refresh) + "\"/>");
    output.append("</head><body>");
    if (refresh > 0) {
      output.append("(Auto-refreshing page every " +
                    base::IntToString(refresh) + "s)");
    } else {
      output.append("(To auto-refresh this page: about:network/&lt;secs&gt;)");
    }

    output.append("<h3>Ethernet:</h3><table border=1>");
    if (ethernet_ && ethernet_enabled()) {
      output.append("<tr>" + ToHtmlTableHeader(ethernet_) + "</tr>");
      output.append("<tr>" + ToHtmlTableRow(ethernet_) + "</tr>");
    }

    output.append("</table><h3>Wifi:</h3><table border=1>");
    for (size_t i = 0; i < wifi_networks_.size(); ++i) {
      if (i == 0)
        output.append("<tr>" + ToHtmlTableHeader(wifi_networks_[i]) + "</tr>");
      output.append("<tr>" + ToHtmlTableRow(wifi_networks_[i]) + "</tr>");
    }

    output.append("</table><h3>Cellular:</h3><table border=1>");
    for (size_t i = 0; i < cellular_networks_.size(); ++i) {
      if (i == 0)
        output.append("<tr>" + ToHtmlTableHeader(cellular_networks_[i]) +
            "</tr>");
      output.append("<tr>" + ToHtmlTableRow(cellular_networks_[i]) + "</tr>");
    }

    output.append("</table><h3>Remembered Wifi:</h3><table border=1>");
    for (size_t i = 0; i < remembered_wifi_networks_.size(); ++i) {
      if (i == 0)
        output.append(
            "<tr>" + ToHtmlTableHeader(remembered_wifi_networks_[i]) +
            "</tr>");
      output.append("<tr>" + ToHtmlTableRow(remembered_wifi_networks_[i]) +
          "</tr>");
    }

    output.append("</table></body></html>");
    return output;
  }
