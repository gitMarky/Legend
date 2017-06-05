
local has_hook_anchored;
local last_point;

// Call this to break the rope.
public func BreakRope()
{
	RemoveObject();
	return;
}

/*-- Rope Callbacks --*/

// From the rope library: to be overloaded for special segment behaviour.
private func CreateSegment(int index, object previous)
{
	if (index == 0)
		return;
	var segment;
	segment = CreateObjectAbove(Item_GrapplerChain);
	return segment;
}

func GetConnectStatus(){ return true; }

/*-- Rope connecting --*/

// Connects two objects to the rope, but the length will vary on their positions.
public func Connect(object obj1, object obj2)
{
	has_hook_anchored = false;
	SetAction("Hide");
	AddEffect("IntHang", this, 1, 1, this);
	return;
}


// Callback form the hook, when it hits ground.
public func HookAnchored()
{
	has_hook_anchored = true;
}

public func HookRemoved()
{
	DrawIn();
}

/* Callback form the rope library */
public func MaxLengthReached()
{
}

// Adjust speed on swinging.
public func DoSpeed(int value)
{
}


public func FxDrawInTimer()
{
}

public func DrawIn(bool no_control)
{
	if (!GetEffect("DrawIn", this))
	{
		AddEffect("DrawIn", this, 1, 1, this);
	}
}

public func AdjustClonkMovement()
{
}

public func UpdateLines()
{
	return;
}

public func GetClonkAngle()
{
	return 0;
}

public func GetClonkPos()
{
}

public func GetClonkOff()
{
}

public func SetLineTransform(object obj, int r, int xoff, int yoff, int length, int layer, int mirror_segments)
{
	if (!mirror_segments) 
		mirror_segments = 1;
	var fsin = Sin(r, 1000), fcos = Cos(r, 1000);
	// Set matrix values.
	obj->SetObjDrawTransform (
		+fcos * mirror_segments, +fsin * length / 1000, xoff,
		-fsin * mirror_segments, +fcos * length / 1000, yoff, layer
	);
}

/*-- Library Overloads --*/


// Only the grappler is stored.
public func SaveScenarioObject() { return false; }

local ActMap = {
		Hide = {
			Prototype = Action,
			Name = "Hide",
		},
};

local Name = "$Name$";
