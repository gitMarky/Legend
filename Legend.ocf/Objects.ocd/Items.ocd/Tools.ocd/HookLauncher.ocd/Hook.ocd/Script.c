/**
	Grapple Hook
	The hook can be shot with the launcher. On impact the hook will stick to wooden or collectible objects.
	
	@author Marky
*/

/*-- Properties --*/

local chain; // The chain is the connection between the hook and the bow.
local user;
local launcher;

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
