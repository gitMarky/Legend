/**
	Grapple Hook
	The hook can be shot with the launcher. On impact the hook will stick to wooden or collectible objects.
	
	@author Marky
*/

/*-- Properties --*/

local chain; // The chain is the connection between the hook and the bow.
local user;
local launcher;
local fx_control;

local Name = "$Name$";
local Plane = 300;


// Only the launcher is stored.
public func SaveScenarioObject() { return false; }


public func GetChain() { return chain; }


public func GetHookTarget()
{
	var fx = GetEffect("StickToTarget", this);
	if (fx)
	{
		return fx.stick_to;
	}
}


/*-- Callbacks --*/

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


public func Hit()
{
	if (GetEffect("InFlight",this))
	{
		Sound("Objects::Arrow::HitGround");
	}
	if (chain)
	{
		chain->DrawIn();
	}
}


public func HitObject(object target)
{
	if (target == user) return;

	RemoveEffect("InFlight", this);
	RemoveEffect("HitCheck",this);

	Stun(target);
	StickTo(target);

	if (chain)
	{
		chain->DrawIn();
	}
}


public func Entrance(object container)
{
	if (container->GetID() == Item_Grappler) return;
	if (chain)
	{
		chain->BreakChain();
	}
	RemoveObject();
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


/*-- Internals --*/

private func Stun(object target)
{
	if (WeaponCanHit(target))
	{
		// Determine damage to obj from speed and arrow strength.
		if (target->GetAlive())
			Sound("Hits::ProjectileHitLiving?");
		else
			Sound("Objects::Arrow::HitGround");

		target->~OnProjectileHit(this);
		
		// TODO: actual stun
		//WeaponDamage(target, 0, FX_Call_EngObjHit, true);
		//WeaponTumble(target, this->TumbleStrength());
	}
}


private func StickTo(object target)
{
	if (CanStickTo(target) && !GetEffect("StickToTarget", this))
	{
		CreateEffect(StickToTarget, 1, 1, target);
	}
}


public func CanStickTo(object target) // callback from IsProjectileTarget <- stupid code structure, but why not :)
{
	return target.Collectible || target->~IsHookLauncherTarget();
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


local StickToTarget = new Effect
{
	Construction = func (object stick_to)
	{
		this.prec = 1000;
		this.stick_to = stick_to;
		this.dx = Target->GetX(this.prec) - stick_to->GetX(this.prec);
		this.dy = Target->GetY(this.prec) - stick_to->GetY(this.prec);	
	},
	
	Timer = func ()
	{
		if (this.stick_to)
		{
			Target->SetPosition(this.stick_to->GetX(this.prec) + this.dx, this.stick_to->GetY(this.prec) + this.dy, true, this.prec);
		}
		else
		{
			return FX_Execute_Kill;
		}
	},
};


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
