
/* Creation / Destruction */

local gui_health;

private func Construction()
{
	gui_health = {};
	
	var max_hearts = 20;
	var row_hearts = 10;
	var heart_size = 10;
	var grid_width =  heart_size * row_hearts;
	var grid_height = heart_size * max_hearts / row_hearts;
	
	gui_health.layout =
	{
		Target = this,
		Player = GetOwner(),
		Style = GUI_Multiple | GUI_TightGridLayout,
		Right = ToEmString(grid_width),
		Bottom = ToEmString(grid_height),
		Priority = 1,
		BackgroundColor = RGBa(0, 150, 0, 100),
	};

	gui_health.ID = GuiOpen(gui_health.layout);

	for (var index = 0; index < max_hearts; ++index)
	{
		var heart = AssembleHeartSymbol(index);
		
		GuiUpdate({_new_icon = heart}, gui_health.ID);
	}

	UpdateHearts();

	return _inherited(...);
}

private func AssembleHeartSymbol(int index)
{
	var heart_size = 10;
	return {
		Player = NO_OWNER, // hide initially,
		Right = ToEmString(heart_size),
		Bottom = ToEmString(heart_size),
		ID = GetHeartID(index),
		Priority = 1 + index,
		// additional icons
		scaled = { // scaleable subwindow
			empty = {
				Symbol = Icon_Heart,
				GraphicsName = "Broken",
				Priority = 11,
			},
			filled = {
				Symbol = Icon_Heart,
				Priority = 12,
			},
			BackgroundColor = RGBa(10 * index, 0, 0, 100),
		},
		multiplier = index, // info
	};
}


private func Destruction()
{
	if (gui_health.ID) GuiClose(gui_health.ID);

	_inherited(...);
}

private func GetHeartID(int index)
{
	return 1000 + index;
}


private func UpdateHearts()
{
	var cursor = GetCursor(GetOwner());
	if (!cursor) return;
	
	var health = cursor->GetEnergy();
	
	var max_hearts = 20;
	var health_per_heart = 4;
	for (var heart = 0; heart < max_hearts; ++heart)
	{
		var heart_offset = health_per_heart * heart;
		var relative_health = BoundBy(health - heart_offset, 0, health_per_heart);
		
		var update = {Player = NO_OWNER, scaled = {empty = {}, filled = {}}};
		
		if (health > heart_offset)
		{
			update.Player = GetOwner(); // display
			update.scaled.filled.Text = Format("%d", relative_health);
		}
		
		GuiUpdate(update, gui_health.ID, GetHeartID(heart));
	}
}
