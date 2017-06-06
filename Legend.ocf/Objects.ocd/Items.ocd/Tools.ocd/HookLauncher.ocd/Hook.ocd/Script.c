/**
	Grapple Hook
	The hook can be shot with the launcher. On impact the hook will stick to wooden or collectible objects.
	
	@author Marky
*/

local chain; // The chain is the connection between the hook and the bow.
local user;
local launcher;
local fx_control;

public func GetChain() { return chain; }


public func Launch(int angle, int strength, int reach, object shooter, object source)
{
	Exit();

	chain = CreateObject(Item_GrapplerChain);

	chain->Connect(this, source);
	user = shooter;
	launcher = source;

	var xdir = +Sin(angle, strength) + shooter->GetXDir();
	var ydir = -Cos(angle, strength) + shooter->GetYDir();
	SetXDir(xdir);
	SetYDir(ydir);
	SetR(angle);
	Sound("Objects::Arrow::Shoot?");

	AddEffect("HitCheck", this, 1,1, nil, nil, shooter);
	CreateEffect(InFlight, 1, 1, reach, xdir, ydir);
}


public func Destruction()
{
	if (chain)
		chain->HookRemoved();
}


private func Stick()
{
	if (GetEffect("InFlight",this))
	{
		Sound("Objects::Arrow::HitGround");
		RemoveEffect("HitCheck",this);
		RemoveEffect("InFlight", this);
	
		SetXDir(0);
		SetYDir(0);
		SetRDir(0);

		// Stick in landscape (vertex 3-7)
		SetVertex(2, VTX_X,  0, 2);
		SetVertex(2, VTX_Y, -6, 2);
		SetVertex(3, VTX_X, -3, 2);
		SetVertex(3, VTX_Y, -4, 2);
		SetVertex(4, VTX_X,  3, 2);
		SetVertex(4, VTX_Y, -4, 2);
		SetVertex(5, VTX_X,  4, 2);
		SetVertex(5, VTX_Y, -1, 2);
		SetVertex(6, VTX_X, -4, 2);
		SetVertex(6, VTX_Y, -1, 2);
		
		// Stick successful?
		if (!Stuck())
		{
			// If not, draw in to prevent hook from dragging you down
			if (launcher) launcher->DrawChainIn();
			return true;
		}
		
		// Draw in possible other active launchers the user is using once this hook hits a solid area and sticks.
		for (var obj in FindObjects(Find_ID(launcher->GetID()), Find_Container(user)))
			if (obj != launcher)
				obj->DrawChainIn();
	}
}


public func HitObject(object target)
{
	if (target == user) return;
	
	
	/*
	// Determine damage to obj from speed and arrow strength.
	var relx = GetXDir() - obj->GetXDir();
	var rely = GetYDir() - obj->GetYDir();
	var speed = Sqrt(relx * relx + rely * rely);
	var dmg = ArrowStrength() * speed * 1000 / 100;
	
	if (WeaponCanHit(obj))
	{
		if (obj->GetAlive())
			Sound("Hits::ProjectileHitLiving?");
		else
			Sound("Objects::Arrow::HitGround");
		
		obj->~OnProjectileHit(this);
		WeaponDamage(obj, dmg, FX_Call_EngObjHit, true);
		WeaponTumble(obj, this->TumbleStrength());
	}
	*/

	// Stick does something unwanted to controller.
	// TODO: Stick only in wooden objects
	/*
	if (this) 
	{
		Stick();
	}
	*/
	if (chain)
	{
		chain->DrawIn();
	}
}


public func Hit()
{
	if (GetEffect("InFlight",this))
	{
		Sound("Objects::Arrow::HitGround");
	}
	//Stick();
	if (chain)
	{
		Log("Hit->DrawIn");
		chain->DrawIn();
	}
}


local InFlight = new Effect 
{
	Construction = func(int reach, int xdir, int ydir)
	{
		this.reach = reach;
		this.xdir = xdir;
		this.ydir = ydir;
	},

	Timer = func()
	{
		var distance = Distance(Target.launcher->GetX(), Target.launcher->GetY(), Target->GetX(), Target->GetY());
		
		Target->SetR(Angle(Target.launcher->GetX(), Target.launcher->GetY(), Target->GetX(), Target->GetY()));
		
		if (this.reach <= distance)
		{
			Target->SetSpeed();
			if (Target.chain)
			{
				Target.chain->DrawIn();
			}
			return FX_Execute_Kill;
		}
		else
		{
			Target->SetSpeed(this.xdir, this.ydir);
			return FX_OK;
		}
	}
};


public func Entrance(object container)
{
	if (container->GetID() == Item_Grappler) return;
	if (chain)
	{
		chain->BreakChain();
	}
	RemoveObject();
	return;
}


public func OnChainBreak()
{
	// Remove control effect for the grapple bow, but only if it exists.
	// Otherwise RemoveEffect with fx_control == nil removes another effect in the user.
	if (fx_control)
	{
		RemoveEffect(nil, user, fx_control);
	}
	RemoveObject();
}


/*-- Grapple chain controls --*/

public func FxIntGrappleControlControl(object target, proplist effect, int ctrl, int x, int y, int strength, bool repeat, int status)
{
	if (status == CONS_Moved) return false;
	var release = status == CONS_Up;
	// Cancel this effect if user is now attached to something.
	if (target->GetProcedure() == "ATTACH") 
	{
		RemoveEffect(nil, target, effect);
		return false;
	}

	if (ctrl != CON_Up && ctrl != CON_Down && ctrl != CON_Right && ctrl != CON_Left)
		return false;

	if (ctrl == CON_Right)
	{
		effect.mv_right = !release;
		if (release)
		{
			if (effect.lastkey == CON_Right)
			{
		    	target->SetDir(0);
		    	target->UpdateTurnRotation();
			}
			effect.lastkey = CON_Right;
			effect.keyTimer = 10;
		}
	}
	if (ctrl == CON_Left)
	{
		effect.mv_left = !release;
		if (release)
		{
			if (effect.lastkey == CON_Left)
			{
		    	target->SetDir(1);
		    	target->UpdateTurnRotation();
			}
			effect.lastkey = CON_Left;
			effect.keyTimer = 10;
		}
	}
	if (ctrl == CON_Up)
	{
		effect.mv_up = !release;
	}
	if (ctrl == CON_Down)
	{
		effect.mv_down = !release;
	}
	
	// Never swallow the control.
	return false;
}


// Effect for smooth movement.
public func FxIntGrappleControlTimer(object target, proplist effect, int time)
{
	// Cancel this effect if user is now attached to something
	// this check is also in the timer because on a high control rate
	// (higher than 1 actually), the timer could be called first
	if (target->GetProcedure() == "ATTACH")
		return FX_Execute_Kill;
	// Also cancel if the user is contained
	if (target->Contained())
		return FX_Execute_Kill;
		
	return FX_OK;
}


public func FxIntGrappleControlOnCarryHeavyPickUp(object target, proplist effect, object heavy_object)
{
	// Remove the control effect when a carry-heavy object is picked up.
	// The chain will then be drawn in automatically.
	RemoveEffect(nil, target, effect);
	return;
}


public func FxIntGrappleControlRejectCarryHeavyPickUp(object target, proplist effect, object heavy_object)
{
	// Block picking up carry-heavy objects when this user is hanging on a chain.
	return true;
}


public func FxIntGrappleControlStop(object target, proplist effect, int reason, int tmp)
{
	if (tmp) 
		return FX_OK;
	target->SetTurnType(0);
	target->StopAnimation(target->GetRootAnimation(10));
	if (!target->GetHandAction())
		target->SetHandAction(0);
	
	// If the hook is not already drawing in, break the chain.
	if (!GetEffect("DrawIn", this->GetChain()))
	{
		Log("GrappleControl->BreakChain");
		this->GetChain()->BreakChain();
	}
	return FX_OK;
}


// Only the launcher is stored.
public func SaveScenarioObject() { return false; }

/*-- Properties --*/


local Name = "$Name$";
local Plane = 300;
