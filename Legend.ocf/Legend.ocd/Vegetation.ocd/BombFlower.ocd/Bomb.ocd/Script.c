#include Weapon_Bomb

/*-- Usage --*/

public func ControlUse(object clonk, int x, int y)
{
	StartRoll(clonk);
	return true;
}


/*-- Engine Callbacks --*/

public func Hit2(int x, int y)
{
	// explode if velocity is greater than 30
	if (Distance(x, y) >= 300)
	{
		Detonate();
	}
}


/*-- Mechanism --*/

local FxBombRoll = new Effect
{
	Construction = func ()
	{
		this.angle = 0;
		this.tilt = RandomX(-3, -1) * 20;
	},

	Timer = func ()
	{
		var circumference = 314 * Target->GetDefHeight();
		this.angle += Target->GetXDir(100) * 360 / circumference;

		UpdateRotation();
		return FX_OK;
	},
	
	UpdateRotation = func ()
	{
		Target->SetR();
		Target.MeshTransformation = Trans_Mul(Trans_Rotate(this.angle, 0, 0, 1), Trans_Rotate(this.tilt, 1, 0, 0));
	},
};


public func StartRoll(object clonk)
{
	this->Exit(5 * clonk->GetCalcDir(), 7);
	this->SetXDir(clonk->GetXDir() + 20 * clonk->GetCalcDir());
	this->SetYDir(clonk->GetYDir());
	this->Roll();
}


public func IsRolling()
{
	return GetEffect("FxBombRoll", this);
}


public func Roll()
{
	if (!IsRolling())
	{
		CreateEffect(FxBombRoll, 1, 1);		
	}
}

/*-- Properties --*/

local Collectible = true;
local Name = "$Name$";
local Description = "$Description$";
