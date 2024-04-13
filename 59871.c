rend_authorized_client_free(rend_authorized_client_t *client)
{
  if (!client)
    return;
  if (client->client_key)
    crypto_pk_free(client->client_key);
  if (client->client_name)
    memwipe(client->client_name, 0, strlen(client->client_name));
  tor_free(client->client_name);
  memwipe(client->descriptor_cookie, 0, sizeof(client->descriptor_cookie));
  tor_free(client);
}
