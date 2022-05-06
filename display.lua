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

-- GRID DRAWING

-- 8x8 grid
local grid_cols = 8
local grid_rows = 8

-- size: 160x160 
-- Bounds TL (10, 20)     TR (width - 10, 20)
--        BL (10, height) BR (width - 10, height)
local margin_left = 10
local margin_top = 20

local square_size = 160 / math.max(grid_cols, grid_rows)



-- converts string of hex into byte array
local function decode_hex(hex_str)
  local data = {}

  -- convert hex character to its corressponding value
  local get_hex_value = function (byte)
    if byte >= 65 and byte <= 70 then
      return 10 + byte - 65
    elseif byte >= 97 and byte <= 102 then
      return 10 + byte - 97
    end

    return byte - 48
  end

  for i=1, string.len(hex_str)/2 do
    local index = i * 2
    local upper = get_hex_value(string.byte(hex_str, index - 1))
    local lower = get_hex_value(string.byte(hex_str, index    ))

    data[i] = upper * 16 + lower
  end

  return data
end

local function parse_grid_data(data)
  -- RGB + character
  local square_data_size = 4
  local grid_data = {}

  for i = 1, (#data) / square_data_size do
    local square_data = {
      color = { 
        r = data[i * square_data_size - 3],
        g = data[i * square_data_size - 2],
        b = data[i * square_data_size - 1], 
      },
      ch = data[i * square_data_size]
    }

    grid_data[i] = square_data
  end

  return grid_data
end

function draw_grid(property_values, display_info, dirty_rect)
  local width = display_info.width
	local height = display_info.height

  -- Draw Header
  jbox_display.draw_rect(
    {left=0, top = 0, right=width, bottom=15}, -- bounds
    {r=120, g=120, b=120})                     -- color
  
  draw_string(5, 7, "GRID", 
    {r=205, g=205, b=205}, "Big bold LCD font")

  -- sample string: "fcba034105c0e66d097d286f960c7f6ef5053167"
  local bytes = decode_hex(property_values[1])
  local square_data = parse_grid_data(bytes)

  for i, square in ipairs(square_data) do
    local x = ((i - 1) % grid_cols) * square_size + margin_left
    local y = math.floor((i - 1) / grid_cols) * square_size + margin_top

    jbox_display.draw_rect(
      { left=x, top = y, right=x + square_size, bottom=y + square_size },
      square.color
    )
    
    if square.ch < 0x80 then
      jbox_display.draw_text(
        {left=x, top=y, right=x + square_size, bottom=y + square_size},
        "center",
        string.char(square.ch),
        "Big bold LCD font",
        {r=205, g=205, b=205}
      )
    end

  end

end

-- GRID Gestures

-- gesture protocol:
-- identifier (0x1) index (0x1) salt (0x1 or 0x2)
local press_id = "01"
local release_id = "00"

-- Apply salt to a parameter
-- this makes it so that duplicate messages will differ
-- by alternating the length with a padded string
local function apply_salt(text)
  if (string.len(text)) % 4 < 2 then
    return ""
  else
    return "00"
  end
end


local function get_grid_index(x , y, width, height) 
  -- Check bounds
  if x < margin_left or y < margin_top or x >= width - margin_left then
    return nil
  end

  local grid_x = math.floor((x - margin_left) / square_size)
  local grid_y = math.floor((y - margin_top ) / square_size)

  local index = grid_x + grid_cols * grid_y

  return index
end

-- Gesture handling
function handle_press(property_values, display_info, gesture_info, custom_data)
  local width = display_info.width
	local height = display_info.height
  local x = gesture_info.start_point.x
  local y = gesture_info.start_point.y

  local index = get_grid_index(x, y, width, height)
  if index == nil then
    return {}
  end

  -- convert to hex
  index = string.format("%02X", index)
  property_values[1] = nil -- nil implies no change
  property_values[2] = press_id .. index .. apply_salt(property_values[2])

  -- return updated values
  return {
    gesture_ui_name = jbox.ui_text("text_GridClick"),
    property_changes= property_values
  }
end

function handle_release(property_values, display_info, gesture_info, custom_data)
  local width = display_info.width
	local height = display_info.height
  local x = gesture_info.start_point.x
  local y = gesture_info.start_point.y

  local index = get_grid_index(x, y, width, height)
  if index == nil then
    return {}
  end

  -- convert to hex
  index = string.format("%02X", index)
  property_values[1] = nil -- nil implies no change 
  property_values[2] = release_id .. index .. apply_salt(property_values[2])

  -- return updated values
  return {
    gesture_ui_name = jbox.ui_text("text_GridRelease"),
    property_changes= property_values
  }
end

-- gesture_grid provides reason with the neccesary handlers for each event.
function gesture_grid(property_values, display_info, gesture_start_point)
  local gesture_definitions = {
    handlers = {
      on_tap = "handle_press",
      on_release = "handle_release"
    }
  }

  return gesture_definitions
end 