  WifiNetwork* GetWifiNetworkByName(const std::string& name) {
    for (size_t i = 0; i < wifi_networks_.size(); ++i) {
      if (wifi_networks_[i]->name().compare(name) == 0) {
        return wifi_networks_[i];
      }
    }
    return NULL;
  }
