static av_always_inline uint8_t color_transform_delta(uint8_t color_pred,
                                                      uint8_t color)
{
    return (int)ff_u8_to_s8(color_pred) * ff_u8_to_s8(color) >> 5;
}
