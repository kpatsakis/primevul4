void OverlayWindowViews::UpdatePlayPauseControlsSize() {
  UpdateButtonSize();
  play_pause_controls_view_->SetSize(button_size_);
  play_pause_controls_view_->SetImage(
      views::Button::STATE_NORMAL,
      gfx::CreateVectorIcon(vector_icons::kPlayArrowIcon,
                            button_size_.width() / 2, kControlIconColor));
  gfx::ImageSkia pause_icon = gfx::CreateVectorIcon(
      vector_icons::kPauseIcon, button_size_.width() / 2, kControlIconColor);
  play_pause_controls_view_->SetToggledImage(views::Button::STATE_NORMAL,
                                             &pause_icon);
  const gfx::ImageSkia play_pause_background = gfx::CreateVectorIcon(
      kPictureInPictureControlBackgroundIcon, button_size_.width(), kBgColor);
  play_pause_controls_view_->SetBackgroundImage(
      kBgColor, &play_pause_background, &play_pause_background);
}
