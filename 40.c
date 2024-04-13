  virtual WifiNetwork* FindWifiNetworkByPath(
      const std::string& path) {
    return GetWirelessNetworkByPath(wifi_networks_, path);
  }
