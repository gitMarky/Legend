#include Library_CarryHeavy
#include Library_Stackable

/*-- Carry heavy stuff --*/

public func GetCarryTransform(clonk)
{
	if (GetCarrySpecial(clonk))
	{
		return Trans_Identity();
	}
	else
	{
		return Trans_Rotate(90, 0, 0, 1);
	}
}

public func GetCarryBone() { return "Base"; }

public func GetCarryPhase() { return 500; }


/*-- Engine Callbacks --*/

public func Hit(int x, int y)
{
	StonyObjectHit(x,y);
}


/*-- Usage --*/

public func ControlUse(object clonk, int x, int y)
{
	Fuse();
	return true;
}


/*-- Mechanism --*/

local FuseTime = 140;

local FxBombFuse = new Effect
{
	Construction = func (int fuse)
	{
		this.fuse = fuse;
	},
	
	Timer = func (int time)
	{
		if (time > this.fuse)
		{
			Target->Detonate();
			return FX_Execute_Kill;
		}
		else
		{
			// countdown flash effect
			var flash_interval = 15;
			var remaining = this.fuse - time;

			if (remaining < this.fuse / 2)
			{
				flash_interval = 7;
			}
			
			if (time % flash_interval == 0)
			{
				Target->Flash(RGB(255, 0, 0), 2 * flash_interval / 3);
			}
			
			// fuse effect
			var fuse_length = 5 - 2 * time / this.fuse;
			var x = +Sin(Target->GetR(), fuse_length);
			var y = -Cos(Target->GetR(), fuse_length);
			
			if (Target->Contained())
			{
				x += -4 + 8 * Target->Contained()->GetDir();
			}

			if (time < this.fuse - 20)
			{
				Target->CreateParticle("Fire", x, y, PV_Random(x - 5, x + 5), PV_Random(y - 15, y - 5), PV_Random(10, 20), Particles_Glimmer(), 3);
			}

			return FX_OK;
		}
	},
};


public func IsFusing()
{
	return GetEffect("FxBombFuse", this);
}


public func Fuse()
{
	if (!IsFusing())
	{
		Sound("Fire::Fuse");
		Sound("Fire::FuseLoop", nil, nil, nil, +1);
		CreateEffect(FxBombFuse, 1, 1, FuseTime);		
	}
}


public func Detonate()
{
	Explode(30, false, this->GetWeaponDamageAmount());
}

/*-- Stacking --*/

public func IsStackable(){ return ; }
public func MaxStackCount()
{
	if (GetType() == C4V_C4Object && Contained() && Contained()->~IsBombBag())
	{
		return Contained()->BombCapacity();
	}
	else
	{
		return 1;
	}
}


/*-- Weapon Properties --*/

public func IsWeapon() { return true; }
public func GetWeaponDamageAmount(){ return 8; }

public func IsBomb(){ return true; }

/*-- Properties --*/

public func IsExplosive() { return true; }

local Collectible = true;
local Name = "$Name$";
local Description = "$Description$";
