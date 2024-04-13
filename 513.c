 void OverlayWindowViews::ButtonPressed(views::Button* sender,
                                       const ui::Event& event) {
  if (sender == close_controls_view_.get())
    controller_->Close(true /* should_pause_video */,
                       true /* should_reset_pip_player */);

  if (sender == play_pause_controls_view_.get())
    TogglePlayPause();

  if (sender == first_custom_controls_view_.get())
    controller_->CustomControlPressed(first_custom_controls_view_->id());

  if (sender == second_custom_controls_view_.get())
    controller_->CustomControlPressed(second_custom_controls_view_->id());
}
