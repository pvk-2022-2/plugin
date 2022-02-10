format_version = "1.0"

function draw_terminal(property_values, display_info, dirty_rect) 
  local width = display_info.width
	local height = display_info.height

  local str = property_values[1]
  assert(str ~= nil)

  jbox_display.draw_text(
  {left=10, top=10, right=width, bottom=10}, "left",
  "BRUH AMONGUS NO WAY",
  "LCD font",{r=205, g=205, b=205})

  jbox_display.draw_text(
  {left=10, top=40, right=width, bottom=40}, "left",
  str,
  "LCD font",{r=205, g=205, b=205})
end