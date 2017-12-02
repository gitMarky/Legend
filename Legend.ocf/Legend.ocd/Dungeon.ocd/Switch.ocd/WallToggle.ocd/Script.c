#include Library_Switch

/* -- Functionality -- */


// Getting hit toggles this switch, so it should be a target
public func IsProjectileTarget(object projectile, object shooter){ return true; }

// Getting hit toggles this switch
public func OnProjectileHit(object projectile)
{
	SetEyeClosed(true, projectile);
}

private func SetEyeClosed(bool state, object by_user)
{
	// Update graphics
	var gfx;
	if (state)
	{
		gfx = "closed";
	}
	else
	{
		gfx = "opened";
	}
	
	UpdateGraphics(gfx);

	// Do the usual
	if (state)
	{
		Sound("Switch::SwitchLockIn");
	}
	ScheduleCall(this, this.SetSwitchState, 25, nil, state, by_user);
}


// Sets the skin name. Possible skins:
// - nil
// - gold
public func SetSkin(string name)
{
	skin_name = name;
	UpdateGraphics();
}

private func UpdateGraphics(string gfx)
{
	skin_gfx = gfx;
	var material = "SwitchEye_";
	if (skin_name) material = Format("%s%s_", material, skin_name);
	material = Format("%s%s", material, gfx);

	SetMeshMaterial(material);
}

/* -- Properties -- */

local Name = "$Name$";
local Description = "$Description$";
local skin_name;
local skin_gfx = "opened";


/*-- Saving --*/

public func SaveScenarioObject(proplist props)
{
	if (!inherited(props, ...)) return false;
	if (skin_name) props->AddCall("Invert", this, "SetSkinName", skin_name);
	return true;
}

/* -- Editor -- */

func Definition(def)
{
	def.MeshTransformation = Trans_Scale(2000, 2000, 2000);
	// Editor properties
	if (!def.EditorProps) def.EditorProps = {};
	def.EditorProps.action_open = new UserAction.Prop { Name="$ActionOpen$" };
	def.EditorProps.action_close = new UserAction.Prop { Name="$ActionClose$" };
	// Actions
	if (!def.EditorActions) def.EditorActions = {};
	def.EditorActions.SwitchOpen = { Name = "$SwitchOpen$", Command = "SetEyeClosed(false)" };
	def.EditorActions.SwitchClose = { Name = "$SwitchClosed$", Command = "SetEyeClosed(true)" };
	return _inherited(def, ...);
}
