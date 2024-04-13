void OverlayWindowViews::SetUpViews() {
  gfx::Rect larger_window_bounds = GetBounds();
  larger_window_bounds.Inset(-1, -1);
  window_background_view_->SetSize(larger_window_bounds.size());
  window_background_view_->SetPaintToLayer(ui::LAYER_SOLID_COLOR);
  GetWindowBackgroundLayer()->SetColor(SK_ColorBLACK);

  controls_scrim_view_->SetSize(GetBounds().size());
  controls_scrim_view_->SetPaintToLayer(ui::LAYER_SOLID_COLOR);
  GetControlsScrimLayer()->SetColor(gfx::kGoogleGrey900);
  GetControlsScrimLayer()->SetOpacity(0.43f);

  controls_parent_view_->SetPaintToLayer(ui::LAYER_TEXTURED);
  controls_parent_view_->SetSize(GetBounds().size());
  controls_parent_view_->layer()->SetFillsBoundsOpaquely(false);
  controls_parent_view_->set_owned_by_client();

  close_controls_view_->SetPaintToLayer(ui::LAYER_TEXTURED);
  close_controls_view_->layer()->SetFillsBoundsOpaquely(false);
  close_controls_view_->set_owned_by_client();

  video_view_->SetPaintToLayer(ui::LAYER_TEXTURED);

  play_pause_controls_view_->SetImageAlignment(
      views::ImageButton::ALIGN_CENTER, views::ImageButton::ALIGN_MIDDLE);
  play_pause_controls_view_->SetToggled(controller_->IsPlayerActive());
  play_pause_controls_view_->set_owned_by_client();

#if defined(OS_CHROMEOS)
  resize_handle_view_->SetPaintToLayer(ui::LAYER_TEXTURED);
  resize_handle_view_->layer()->SetFillsBoundsOpaquely(false);
  resize_handle_view_->set_owned_by_client();
#endif

  play_pause_controls_view_->SetFocusForPlatform();  // Make button focusable.
  const base::string16 play_pause_accessible_button_label(
      l10n_util::GetStringUTF16(
          IDS_PICTURE_IN_PICTURE_PLAY_PAUSE_CONTROL_ACCESSIBLE_TEXT));
  play_pause_controls_view_->SetAccessibleName(
      play_pause_accessible_button_label);
  const base::string16 play_button_label(
      l10n_util::GetStringUTF16(IDS_PICTURE_IN_PICTURE_PLAY_CONTROL_TEXT));
  play_pause_controls_view_->SetTooltipText(play_button_label);
  const base::string16 pause_button_label(
      l10n_util::GetStringUTF16(IDS_PICTURE_IN_PICTURE_PAUSE_CONTROL_TEXT));
  play_pause_controls_view_->SetToggledTooltipText(pause_button_label);
  play_pause_controls_view_->SetInstallFocusRingOnFocus(true);

  controls_parent_view_->AddChildView(play_pause_controls_view_.get());
  GetContentsView()->AddChildView(controls_scrim_view_.get());
  GetContentsView()->AddChildView(controls_parent_view_.get());
  GetContentsView()->AddChildView(close_controls_view_.get());
#if defined(OS_CHROMEOS)
  GetContentsView()->AddChildView(resize_handle_view_.get());
#endif

  UpdatePlayPauseControlsSize();
  UpdateControlsVisibility(false);
}
