  virtual void ForgetWifiNetwork(const std::string& service_path) {
    if (!EnsureCrosLoaded())
      return;
    if (DeleteRememberedService(service_path.c_str())) {
      for (WifiNetworkVector::iterator iter =
               remembered_wifi_networks_.begin();
          iter != remembered_wifi_networks_.end();
          ++iter) {
        if ((*iter)->service_path() == service_path) {
          delete (*iter);
          remembered_wifi_networks_.erase(iter);
          break;
        }
      }
      NotifyNetworkManagerChanged();
    }
  }
