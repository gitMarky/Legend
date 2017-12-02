#include Library_Switch

/* -- Functionality -- */


// Getting hit toggles this switch, so it should be a target
public func IsProjectileTarget(object projectile, object shooter){ return !eye_closed; }

// Getting hit toggles this switch
public func OnProjectileHit(object projectile)
{
	SetEyeClosed(true, projectile);
}

private func SetEyeClosed(bool state, object by_user)
{
	eye_closed = state;
	UpdateGraphics();

	// Do the usual
	if (state)
	{
		Sound("Switch::SwitchLockIn");
	}
	ScheduleCall(this, this.SetSwitchState, 25, nil, eye_closed, by_user);
}


// Sets the skin name. Possible skins:
// - nil
// - gold
public func SetSkin(string name)
{
	skin_name = name;
	UpdateGraphics();
}

private func UpdateGraphics()
{
	var gfx;
	if (eye_closed)
	{
		gfx = "closed";
	}
	else
	{
		gfx = "opened";
	}
	var material = "SwitchEye_";
	if (skin_name) material = Format("%s%s_", material, skin_name);
	material = Format("%s%s", material, gfx);

	SetMeshMaterial(material);
}

/* -- Properties -- */

local Name = "$Name$";
local Description = "$Description$";
local skin_name;
local eye_closed = false;


/*-- Saving --*/

public func SaveScenarioObject(proplist props)
{
	if (!inherited(props, ...)) return false;
	if (skin_name) props->AddCall("Skin", this, "SetSkinName", skin_name);
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
