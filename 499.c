void AddQuickEnableWorkItems(const InstallerState& installer_state,
                             const InstallationState& machine_state,
                             const FilePath* setup_path,
                             const Version* new_version,
                             WorkItemList* work_item_list) {
  DCHECK(setup_path ||
         installer_state.operation() == InstallerState::UNINSTALL);
  DCHECK(new_version ||
         installer_state.operation() == InstallerState::UNINSTALL);
  DCHECK(work_item_list);

  const bool system_install = installer_state.system_install();
  bool have_multi_chrome = false;
  bool have_chrome_frame = false;

  const ProductState* product_state = NULL;

  product_state =
      machine_state.GetProductState(system_install,
                                    BrowserDistribution::CHROME_BROWSER);
  if (product_state != NULL && product_state->is_multi_install())
    have_multi_chrome = true;

  product_state =
      machine_state.GetProductState(system_install,
                                    BrowserDistribution::CHROME_FRAME);
  if (product_state != NULL &&
      !product_state->uninstall_command().HasSwitch(
          switches::kChromeFrameReadyMode))
    have_chrome_frame = true;

  const Product* product = NULL;

  if (installer_state.operation() == InstallerState::UNINSTALL) {
    product =
        installer_state.FindProduct(BrowserDistribution::CHROME_BROWSER);
    if (product != NULL && installer_state.is_multi_install())
      have_multi_chrome = false;

    if (installer_state.FindProduct(BrowserDistribution::CHROME_FRAME) != NULL)
      have_chrome_frame = false;
  } else {
    product =
        installer_state.FindProduct(BrowserDistribution::CHROME_BROWSER);
    if (product != NULL && installer_state.is_multi_install())
      have_multi_chrome = true;

    product = installer_state.FindProduct(BrowserDistribution::CHROME_FRAME);
    if (product != NULL && !product->HasOption(kOptionReadyMode))
      have_chrome_frame = true;
  }

  enum QuickEnableOperation {
    DO_NOTHING,
    ADD_COMMAND,
    REMOVE_COMMAND
  } operation = DO_NOTHING;
  FilePath binaries_setup_path;

  if (have_chrome_frame) {
    operation = REMOVE_COMMAND;
  } else if (have_multi_chrome) {
    operation = ADD_COMMAND;
    if (installer_state.operation() == InstallerState::UNINSTALL) {
      product_state =
          machine_state.GetProductState(system_install,
                                        BrowserDistribution::CHROME_BROWSER);
      DCHECK(product_state);
      binaries_setup_path = product_state->uninstall_command().GetProgram();
    } else {
      DCHECK(installer_state.is_multi_install());
      binaries_setup_path =
          installer_state.GetInstallerDirectory(*new_version).Append(
              setup_path->BaseName());
    }
  }

  if (operation != DO_NOTHING) {
    BrowserDistribution* binaries =
        BrowserDistribution::GetSpecificDistribution(
            BrowserDistribution::CHROME_BINARIES);
    std::wstring cmd_key(binaries->GetVersionKey());
    cmd_key.append(1, L'\\').append(google_update::kRegCommandsKey)
        .append(1, L'\\').append(kCmdQuickEnableCf);

    if (operation == ADD_COMMAND) {
      DCHECK(!binaries_setup_path.empty());
      CommandLine cmd_line(binaries_setup_path);
      cmd_line.AppendSwitch(switches::kMultiInstall);
      if (installer_state.system_install())
        cmd_line.AppendSwitch(switches::kSystemLevel);
      if (installer_state.verbose_logging())
        cmd_line.AppendSwitch(switches::kVerboseLogging);
      cmd_line.AppendSwitch(switches::kChromeFrameQuickEnable);
      AppCommand cmd(cmd_line.command_line_string(), true, true);
      cmd.AddWorkItems(installer_state.root_key(), cmd_key, work_item_list);
    } else {
      DCHECK(operation == REMOVE_COMMAND);
      work_item_list->AddDeleteRegKeyWorkItem(installer_state.root_key(),
                                              cmd_key)->set_log_message(
          "removing quick-enable-cf command");
    }
  }
}
