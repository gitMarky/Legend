/**
	Grapple Hook
	The hook can be shot with the grappling bow. On impact the hook will stick to the ground.
	The hook also controls the swinging controls for the clonk.
	
	@author Randrian
*/

local rope; // The rope is the connection between the hook and the bow.
local clonk;
local pull;
local grappler;
local fx_hook;

public func GetRope() { return rope; }


public func New(object new_clonk, object new_rope)
{
	clonk = new_clonk;
	rope = new_rope;
}


public func Launch(int angle, int strength, int reach, object shooter, object bow)
{
	Exit();

	pull = false;
		
	// Create rope.
	rope = CreateObject(Item_GrapplerChain);

	rope->Connect(this, bow);
	clonk = shooter;
	grappler = bow;

	var xdir = +Sin(angle, strength);
	var ydir = -Cos(angle, strength);
	SetXDir(xdir);
	SetYDir(ydir);
	SetR(angle);
	Sound("Objects::Arrow::Shoot?");
	
	AddEffect("HitCheck", this, 1,1, nil, nil, shooter);
	CreateEffect(InFlight, 1, 1, reach, xdir, ydir);
}


public func Destruction()
{
	if (rope)
		rope->HookRemoved();
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
			if (grappler) grappler->DrawRopeIn();
			return true;
		}

		rope->HookAnchored();
		
		// Draw in possible other active grapplers the clonk is using once this hook hits a solid area and sticks.
		for (var obj in FindObjects(Find_ID(GrappleBow), Find_Container(clonk)))
			if (obj != grappler)
				obj->DrawRopeIn();
				
		ScheduleCall(this, "StartPull", 5); // TODO
	}
}


public func StartPull()
{
	pull = true;
	fx_hook = AddEffect("IntGrappleControl", clonk, 1, 1, this);
	if (clonk->GetAction() == "Jump")
	{
		rope->AdjustClonkMovement();
		rope->ConnectPull();
		fx_hook.var5 = 1;
		fx_hook.var6 = 10;
	}
}


public func HitObject(object obj)
{
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
	if (rope)
	{
		Log("HitObject->DrawIn");
		rope->DrawIn();
	}
	return;
}


public func Hit()
{
	if (GetEffect("InFlight",this))
	{
		Sound("Objects::Arrow::HitGround");
	}
	//Stick();
	if (rope)
	{
		Log("Hit->DrawIn");
		rope->DrawIn();
	}
}


local InFlight = new Effect 
{
	Construction = func(int reach, int xdir, int ydir)
	{
		this.reach = reach;
		this.origin_x = Target->GetX();
		this.origin_y = Target->GetY();
		this.xdir = xdir;
		this.ydir = ydir;
	},

	Timer = func()
	{
		var distance = Distance(this.origin_x, this.origin_y, Target->GetX(), Target->GetY());
		
		if (this.reach <= distance)
		{
			Target->SetSpeed();
			if (Target.rope)
			{
				Log("InFlight->DrawIn %d/%d", distance, this.reach);
				Target.rope->DrawIn();
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
	if (rope)
	{
		rope->BreakRope();
	}
	RemoveObject();
	return;
}


public func OnRopeBreak()
{
	// Remove control effect for the grapple bow, but only if it exists.
	// Otherwise RemoveEffect with fx_hook == nil removes another effect in the clonk.
	if (fx_hook)
	{
		RemoveEffect(nil, clonk, fx_hook);
	}
	RemoveObject();
}


/*-- Grapple rope controls --*/

public func FxIntGrappleControlControl(object target, proplist effect, int ctrl, int x, int y, int strength, bool repeat, int status)
{
	if (status == CONS_Moved) return false;
	var release = status == CONS_Up;
	// Cancel this effect if clonk is now attached to something.
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
		if ((target->GetAction() == "Jump" || target->GetAction() == "WallJump") && !release && pull)
			rope->ConnectPull();
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
	// Cancel this effect if clonk is now attached to something
	// this check is also in the timer because on a high control rate
	// (higher than 1 actually), the timer could be called first
	if (target->GetProcedure() == "ATTACH")
		return FX_Execute_Kill;
	// Also cancel if the clonk is contained
	if (target->Contained())
		return FX_Execute_Kill;
		
	return FX_OK;
}


public func FxIntGrappleControlOnCarryHeavyPickUp(object target, proplist effect, object heavy_object)
{
	// Remove the control effect when a carry-heavy object is picked up.
	// The rope will then be drawn in automatically.
	RemoveEffect(nil, target, effect);
	return;
}


public func FxIntGrappleControlRejectCarryHeavyPickUp(object target, proplist effect, object heavy_object)
{
	// Block picking up carry-heavy objects when this clonk is hanging on a rope.
	if (rope->PullObjects())
		return true;
	return false;
}


public func FxIntGrappleControlStop(object target, proplist effect, int reason, int tmp)
{
	if (tmp) 
		return FX_OK;
	target->SetTurnType(0);
	target->StopAnimation(target->GetRootAnimation(10));
	if (!target->GetHandAction())
		target->SetHandAction(0);
	
	// If the hook is not already drawing in, break the rope.
	if (!GetEffect("DrawIn", this->GetRope()))
	{
		Log("GrappleControl->BreakRope");
		this->GetRope()->BreakRope();
	}
	return FX_OK;
}


private func Trans_RotX(int rotation, int ox, int oy)
{
	return Trans_Mul(Trans_Translate(-ox, -oy), Trans_Rotate(rotation, 0, 0, 1), Trans_Translate(ox, oy));
}


// Only the grappler is stored.
public func SaveScenarioObject() { return false; }

/*-- Properties --*/


local Name = "$Name$";
local Plane = 300;
