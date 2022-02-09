format_version = "2.0"

-------------------------------------------------------------------
-- Note! Players need to specify panel_type (new in format_version 2).
-- Players don't have a folded state, so they should NOT provide
-- a folded_front nor a folded_back.
--------------------------------------------------------------------

panel_type = "note_player"

front = {

	Surface_Bg = {
		{ path = "Panel_Front_3U" },
	},
     OnOff = {
        offset = { 30*5, 4*5 },
        { path = "ONOFF maker x5 002", frames = 4 }
    },

    PatchNameSurface = {
            offset = { 127*5, 10*5 },
            { size = { 570, 81 } },
    },
    {
        offset = { 62*5, 7*5 },
        PatchBrowseGroup = {
            offset = { 0, 0 },
            { path = "PatchBrowseGroup" },
        }
    },
    
	---------------------------------------------
	---------------------------------------------
	---  Ratio Knobs (Numerator over Denominator)
	---------------------------------------------
	
	Surface_BeatNumerator = {
		offset = {1600, 300 },
		{ path = "Knob_Green", frames = 63}
	},

	Surface_BeatDenominator = {
		offset = {1600, 700 },
		{ path = "Knob_Blue", frames = 63}
	},
}

back = {
	Surface_Bg = {
		{ path = "Panel_Back_3U" },
	},
    Placeholder = {
        offset = { 2100, 200 },
            { path = "Placeholder" }
    },
    CVJack1 = {
        offset = { 500, 100 },
        { path = "SharedCVJack", frames = 3 }
    },
    CVJack2 = {
        offset = { 500, 400 },
        { path = "SharedCVJack", frames = 3 }
    },
}
