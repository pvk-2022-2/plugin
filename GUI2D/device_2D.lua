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

    Terminal = {
        offset = { 300, 50 },
        { path = "Terminal" }
    },

    Grid = {
        offset = { 2400, 50 },
        { path = "Grid" }
    },

    OnOff = {
        offset = { 30*5, 4*5 },
        { path = "ONOFF maker x5 002", frames = 4 }
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
