void OverlayWindowViews::CreateCustomControl(
    std::unique_ptr<views::ControlImageButton>& control_button,
    const blink::PictureInPictureControlInfo& info,
    ControlPosition position) {
  control_button = std::make_unique<views::ControlImageButton>(this);
  controls_parent_view_->AddChildView(control_button.get());
  control_button->set_id(info.id);
  control_button->set_owned_by_client();

  control_button->SetImageAlignment(views::ImageButton::ALIGN_CENTER,
                                    views::ImageButton::ALIGN_MIDDLE);
  UpdateCustomControlsSize(control_button.get());
  UpdateControlsBounds();

  base::string16 custom_button_label = base::UTF8ToUTF16(info.label);
  control_button->SetAccessibleName(custom_button_label);
  control_button->SetTooltipText(custom_button_label);
  control_button->SetInstallFocusRingOnFocus(true);
  control_button->SetFocusForPlatform();
}
