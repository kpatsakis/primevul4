void OverlayWindowViews::UpdateCustomControlsSize(
    views::ControlImageButton* control_button) {
  if (!control_button)
    return;
  UpdateButtonSize();
  control_button->SetSize(button_size_);
  if (control_button == first_custom_controls_view_.get()) {
    first_custom_controls_view_->SetImage(
        views::Button::STATE_NORMAL,
        gfx::CreateVectorIcon(vector_icons::kPlayArrowIcon,
                              button_size_.width() / 2, kControlIconColor));
  }
  if (control_button == second_custom_controls_view_.get()) {
    second_custom_controls_view_->SetImage(
        views::Button::STATE_NORMAL,
        gfx::CreateVectorIcon(vector_icons::kPauseIcon,
                              button_size_.width() / 2, kControlIconColor));
  }
  const gfx::ImageSkia control_background = gfx::CreateVectorIcon(
      kPictureInPictureControlBackgroundIcon, button_size_.width(), kBgColor);
  control_button->SetBackgroundImage(kBgColor, &control_background,
                                     &control_background);
}
