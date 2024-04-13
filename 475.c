WifiNetwork::WifiNetwork(const ServiceInfo* service)
    : WirelessNetwork(service) {
  encryption_ = service->security;
  passphrase_ = SafeString(service->passphrase);
  identity_ = SafeString(service->identity);
  cert_path_ = SafeString(service->cert_path);
  type_ = TYPE_WIFI;
}
