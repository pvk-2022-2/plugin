format_version = "1.0"

-- draw wrapper with top/left position instead of bounds, and optional font
function draw_string(x, y, str, color, font)
  font = font or "LCD font"
  color = color or {r=205, g=205, b=205}

  jbox_display.draw_text(
    {left=x, top=y, right=x, bottom=y},
    "left",
    str,
    font,
    color
  )
end

function draw_terminal(property_values, display_info, dirty_rect) 
  local width = display_info.width
	local height = display_info.height

  local str = property_values[1]
  assert(str ~= nil)

  -- Draw Header
  jbox_display.draw_rect(
    {left=0, top = 0, right=width, bottom=15}, -- bounds
    {r=120, g=120, b=120})                     -- color

  draw_string(5, 7, "TERMINAL LETS GOOOO", 
    {r=205, g=205, b=205}, "Big bold LCD font")

  draw_string(20, 20, str)
end