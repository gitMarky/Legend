#include Library_CarryHeavy
#include Library_Stackable

/*-- Carry heavy stuff --*/

public func GetCarryTransform(clonk)
{
	if (GetCarrySpecial(clonk))
	{
		return Trans_Translate(1000, -6500, 0);
	}
	else
	{
		return Trans_Mul(Trans_Translate(1500, 0, -1500), Trans_Rotate(180, 1, 0, 0));
	}
}

public func GetCarryPhase() { return 900; }


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
			var x = +Sin(Target->GetR(), 5);
			var y = -Cos(Target->GetR(), 5);

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
	if (Contained() && Contained()->~IsBombBag())
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
