
/*-- Chain mechanism --*/

local chain_segments;
local hook;
local launcher;

local fx_draw;


// Connects two objects to the chain, but the length will vary on their positions.
public func Connect(object hook, object launcher)
{
	SetAction("Hide");
	this.hook = hook;
	this.launcher = launcher;
	chain_segments = [];
	AddTimer(this.UpdateLines, 1);
	return;
}


// Call this to break the rope.
public func BreakChain()
{
	RemoveObject();
}


public func HookRemoved()
{
	RemoveObject();
}


public func Destruction()
{
	if (chain_segments)
	{
		for (var i = 0; i < GetLength(chain_segments); ++i)
		{
			DeleteChainSegment(i);
		}
	}
}


local FxNoBash = new Effect
{
	QueryHitClonk = func (object clonk)
	{
		return true;
	},
	
	Timer = func()
	{
		return FX_Execute_Kill;
	},
};


local FxDrawIn = new Effect
{
	Timer = func ()
	{
		if (!Target.launcher || !Target.hook)
		{
			if (Target.hook)
			{
				Target.hook->RemoveObject();
			}		
			return FX_Execute_Kill;
		}

		// determine objects
		var user = Target.launcher->Contained();

		var pull_to = GetOutmostContainer(Target.launcher);
		var pull_me = GetOutmostContainer(Target.hook);

		var mass_me = Max(pull_me->GetMass(), 1);
		var mass_to = Max(pull_to->GetMass(), 1);
		
		// save user from getting hurt
		pull_me->CreateEffect(Item_GrapplerChain.FxNoBash, 1, 5);

		// parameters

		var precision = 1000;
		var angle = Angle(pull_me->GetX(), pull_me->GetY(), pull_to->GetX(), pull_to->GetY(), precision);
		var distance = Distance(pull_me->GetX(), pull_me->GetY(), pull_to->GetX(), pull_to->GetY());
		var velocity = 70 * Clonk->GetMass();
		var max_velocity = 70;
		
		if (distance < 20) velocity /= 2; // slow down

		// determine velocities

		var vel_me = +velocity / mass_me;
	
		if (IsImmobile(pull_me) || pull_me->~IsHookPullTarget()) // pull the launcher towards the hook?
		{
			vel_me = 0;
		}
		else // sanity checks for stuck hook
		{
			this.hook_xold = this.hook_xnew;
			this.hook_yold = this.hook_ynew;
			this.hook_xnew = Target.hook->GetX();
			this.hook_ynew = Target.hook->GetY();
			
			if (this.hook_xold == this.hook_xnew
			 && this.hook_yold == this.hook_ynew)
			{
				++this.hook_stuck;
			}
			else
			{
				this.hook_stuck = 0;
			}
			
			if (this.hook_stuck > 20)
			{
				Target.hook->RemoveObject();
				return FX_Execute_Kill;
			}
			else if (this.hook_stuck > 10)
			{
				Target.hook->~UnStick();
			}
		}
		
		// calculate velocity

		var remaining = BoundBy(velocity - vel_me * mass_me, 0, velocity);

		var vel_to = -remaining / mass_to;

		pull_to->SetR(angle);

		// update object velocity

		vel_me = Min(vel_me, max_velocity);
		vel_to = Min(vel_to, max_velocity);

		if (vel_me != 0) pull_me->SetSpeed(+Sin(angle, vel_me, precision), -Cos(angle, vel_me, precision));
		if (vel_to != 0)
		{
			if (pull_to.ActMap && pull_to.ActMap.Jump && pull_to->GetProcedure() != DFA_FLIGHT)
			{
				pull_to->SetAction("Jump");
			}
			pull_to->SetSpeed(+Sin(angle, vel_to, precision), -Cos(angle, vel_to, precision));
		}

		if (distance < 10)
		{
			if (pull_me.Collectible && user)
			{
				user->~Collect(pull_me);
			}

			Target.hook->RemoveObject();
			return FX_Execute_Kill;
		}
	},
	
	IsImmobile = func(object target)
	{
		return target->Stuck() || !(target->GetCategory() == C4D_Object || target->GetCategory() == C4D_Vehicle);
	},
	
	GetOutmostContainer = func(object target)
	{
		var outmost = target->Contained() ?? target->~GetHookTarget();
		if (outmost)	
		{
			return GetOutmostContainer(outmost);
		}
		else
		{
			return target;
		}
	},
};


public func DrawIn()
{
	if (!fx_draw)
	{
		fx_draw = CreateEffect(FxDrawIn, 1, 1);
	}
}


/*-- Display --*/

private func CreateChainSegment(int index)
{
	if (!chain_segments[index])
	{
		chain_segments[index] = CreateObject(this->GetID());
	}
}


private func DeleteChainSegment(int index)
{
	if (chain_segments[index]) chain_segments[index]->RemoveObject();
}


private func UpdateLines()
{
	if (!hook || !launcher)
	{
		RemoveTimer(this.UpdateLines);
		return;
	}

	var prec = 1000;
	var dx = hook->GetX(prec) - launcher->GetX(prec);
	var dy = hook->GetY(prec) - launcher->GetY(prec);
	var distance = Distance(dx, dy);
	var angle = Angle(0, 0, dx, dy);
	var segment_height = (GetID()->GetDefHeight() - 1);
	var segment_length = segment_height * 1000;
	var segment_amount = 1 + (distance - (distance % segment_length)) / segment_length;
	// draw visible segments
	for (var i = 0; i < segment_amount; ++i)
	{
		CreateChainSegment(i);
		
		var x = launcher->GetX(prec) + i * dx / segment_amount;
		var y = launcher->GetY(prec) + i * dy / segment_amount;
		
		var length = BoundBy(distance - i * segment_length, 0, segment_length) / segment_height;
	
		// Update the position of the segment
		chain_segments[i]->SetPosition(x, y, true, prec);
		
		// Only apply line transform to the rope segments and not to the hook.
		chain_segments[i]->SetR(angle);
		chain_segments[i]->SetObjDrawTransform(1000, 0, 0, 0, length, segment_height * length / (-2));
	}
	// remove obsolete objects
	for (var i = segment_amount; i < GetLength(chain_segments); ++i)
	{
		DeleteChainSegment(i);
	}
	RemoveHoles(chain_segments);
	return;
}


/*-- Properties --*/

// Only the grappler is stored.
public func SaveScenarioObject() { return false; }


local ActMap = {
	Hide = {
		Prototype = Action,
		Name = "Hide",
	},
};


local Name = "$Name$";
