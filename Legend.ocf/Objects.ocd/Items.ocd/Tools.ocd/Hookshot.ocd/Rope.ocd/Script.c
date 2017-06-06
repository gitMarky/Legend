
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

// Connects two objects to the chain, but the length will vary on their positions.
public func Connect(object obj1, object obj2)
{
	has_hook_anchored = false;
	SetAction("Hide");
	//AddEffect("IntHang", this, 1, 1, this);
	this.obj1 = obj1;
	this.obj2 = obj2;
	return;
}


// Callback form the hook, when it hits ground.
public func HookAnchored()
{
	has_hook_anchored = true;
}

public func HookRemoved()
{
	Log("HookRemoved->DrawIn");
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


local FxDrawIn = new Effect
{
	Timer = func ()
	{
		var pull_to;
		var pull_me;
		if (this.Target.obj1 && this.Target.obj2)
		{
			if (this.Target.obj1->Contained())
			{
				pull_to = this.Target.obj1;
				pull_me = this.Target.obj2;
			}
			else
			{
				pull_to = this.Target.obj2;
				pull_me = this.Target.obj1;
			}

			var precision = 1000;
			var angle = Angle(pull_me->GetX(), pull_me->GetY(), pull_to->GetX(), pull_to->GetY(), precision);
			var distance = Distance(pull_me->GetX(), pull_me->GetY(), pull_to->GetX(), pull_to->GetY());
			var velocity = 70;

			pull_me->SetSpeed(Sin(angle, velocity, precision), -Cos(angle, velocity, precision));

			if (distance < 10)
			{
				pull_me->RemoveObject();
				return FX_Execute_Kill;
			}
		}
	},
};


public func DrawIn()
{
	Log("Drawn in!");
	if (!GetEffect("FxDrawIn", this))
	{
		CreateEffect(FxDrawIn, 1, 1);
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
