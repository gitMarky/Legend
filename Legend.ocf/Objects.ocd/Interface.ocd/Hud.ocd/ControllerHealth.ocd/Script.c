
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Properties

local gui_health;


static const GUI_HEALTH_MARGIN_LEFT_EM = 5;
static const GUI_HEALTH_MARGIN_TOP_EM = 5;

static const GUI_HEALTH_HEART_SIZE_EM = 12;
static const GUI_HEALTH_MAX_HEARTS = 20; // this many hearts total
static const GUI_HEALTH_ROW_HEARTS = 10; // this many hearts per row
static const GUI_HEALTH_PER_HEART = 4; // this many health points per heart

static const GUI_HEALTH_GRID_WIDTH =  GUI_HEALTH_HEART_SIZE_EM * GUI_HEALTH_ROW_HEARTS;
static const GUI_HEALTH_GRID_HEIGHT = GUI_HEALTH_HEART_SIZE_EM * GUI_HEALTH_MAX_HEARTS / GUI_HEALTH_ROW_HEARTS;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Construction / Destruction

private func Construction()
{
	// define GUI

	gui_health = {};
	gui_health.layout =
	{
		Target = this,
		Player = GetOwner(),
		Style = GUI_Multiple | GUI_TightGridLayout,
		Left = ToEmString(GUI_HEALTH_MARGIN_LEFT_EM),
		Top = ToEmString(GUI_HEALTH_MARGIN_TOP_EM),
		Right = Format("%s%s", ToEmString(GUI_HEALTH_MARGIN_LEFT_EM), ToEmString(GUI_HEALTH_GRID_WIDTH)),
		Bottom = Format("%s%s", ToEmString(GUI_HEALTH_MARGIN_TOP_EM), ToEmString(GUI_HEALTH_GRID_HEIGHT)),
		Priority = 1,
		// for debugging BackgroundColor = RGBa(0, 150, 0, 100),
	};
	
	// create GUI

	gui_health.ID = GuiOpen(gui_health.layout);

	for (var index = 0; index < GUI_HEALTH_MAX_HEARTS; ++index)
	{
		var heart = AssembleHeartSymbol(index);
		
		GuiUpdate({_new_icon = heart}, gui_health.ID);
	}

	AddTimer(this.UpdateHearts, 1);

	return _inherited(...);
}


private func Destruction()
{
	if (gui_health.ID) GuiClose(gui_health.ID);

	_inherited(...);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// GUI Components

private func AssembleHeartSymbol(int index)
{
	var heart = {
		Player = NO_OWNER, // hide initially,
		Right = ToEmString(GUI_HEALTH_HEART_SIZE_EM),
		Bottom = ToEmString(GUI_HEALTH_HEART_SIZE_EM),
		ID = GetHeartID(index),
		Priority = 1 + index,
		// additional icons
		scaled = { // scaleable subwindow
			Style = GUI_NoCrop,
			empty = {
				Symbol = Icon_HealthHeart,
				Priority = 10,
			},
			filled = {
				Symbol = Icon_HealthHeart,
				GraphicsName = "0", // no filling by default
				Priority = 11,
			},
			double = {
				Symbol = Icon_HealthHeart,
				GraphicsName = "0", // hidden by default
				Priority = 12,
			},
		},
	};
	
	/*
	var size = 750;
	var scale = {
		Prototype = GUI_BoxLayout,
		Align = {X = GUI_AlignCenter, Y = GUI_AlignCenter},
		Width = size, Height = size,
	};
	
	AddProperties(heart.scaled, GuiCalculateBoxElementPosition(scale));
	*/
	return heart;
}


private func GetHeartID(int index)
{
	return 1000 + index;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// GUI Update

private func UpdateHearts()
{
	var cursor = GetCursor(GetOwner());
	if (!cursor) return;
	

	var health = cursor->GetEnergy();
	var health_max = cursor->GetMaxEnergy();

	if (health != gui_health.last_health || health_max != gui_health.last_health_max)
	{
		gui_health.last_health = health;
		gui_health.last_health_max = health_max;
		DisplayHearts(health, health_max);
	}
}

private func DisplayHearts(int health, int health_max)
{
	for (var heart = 0; heart < GUI_HEALTH_MAX_HEARTS; ++heart)
	{
		var heart_offset = GUI_HEALTH_PER_HEART * heart;
		var relative_health = BoundBy(health - heart_offset, 0, GUI_HEALTH_PER_HEART);
		
		var update = {Player = NO_OWNER, scaled = {filled = {}}};
		
		if (health_max >= (heart_offset + GUI_HEALTH_PER_HEART)) // display
		{
			update.Player = GetOwner();
			update.scaled.filled.GraphicsName = Format("%d", relative_health);
		}
		
		GuiUpdate(update, gui_health.ID, GetHeartID(heart));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Callbacks
