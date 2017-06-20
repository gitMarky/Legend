#include Library_CarryHeavy
#include Library_Plant

local fruit;

private func Construction()
{
	_inherited();

	fruit = { item = nil, mesh = nil};
	GrowFruit(true);
	SetSeedChance(0);
	SetSeedAmount(0);
	
	// rotate
	this.MeshTransformation = Trans_Mul(Trans_Rotate(RandomX(0, 359), 0, 1, 0), Trans_Rotate(RandomX(-5, +5), 0, 0, 1));
	
	// random leaf transformations
	TransformLeaf("Leaf1", RandomX(-40, +40), RandomX(-10, +10), RandomX(-30, +10));
	TransformLeaf("Leaf3", RandomX(-40, +40), RandomX(-10, +10), RandomX(-10, +30));
	TransformLeaf("Leaf2", RandomX(-30, +10), RandomX(-10, +10), RandomX(-40, +40));
	TransformLeaf("Leaf4", RandomX(-10, +30), RandomX(-10, +10), RandomX(-40, +40));
}


public func GrowFruit(bool fullgrown)
{
	fruit.item = CreateContents(BombFlowerBomb);
	fruit.mesh = AttachMesh(fruit.item, "Base", fruit.item->GetCarryBone());
	
	if (!fullgrown)
	{
		fruit.item->SetCon(1);
		fruit.item->StartGrowth(1).Interval = 2;
		
		SetAttachTransform(fruit.mesh, Trans_Scale(1, 1, 1));

		RemoveEffect("FxGrowthMonitor", this);
		CreateEffect(FxGrowthMonitor, 1, 1);
	}
	else
	{
		this.Collectible = true;
	}
	
}


public func PickFruit(object user)
{
	if (fruit.mesh)
	{
		DetachMesh(fruit.mesh);
		fruit.mesh = nil;
	}

	var picked = fruit.item;
	if (fruit.item)
	{
		fruit.item->Exit();
		
		if (user)
		{
			user->Collect(fruit.item);
		}
		
		fruit.item = nil;
		this.Collectible = false; // collectible only while there is fruit
	}
	
	RemoveEffect("FxGrowFruit", this);
	CreateEffect(FxGrowFruit, 1, 200);
	
	return picked;
}


public func RejectEntrance(object into)
{
	var reject = into->~RejectCollect(GetID(), this);

	if (!reject)
	{
		PickFruit(into);
	}
	return true;
}

/*-- Growth monitor --*/

local FxGrowthMonitor = new Effect
{
	Timer = func ()
	{
		if (Target.fruit && Target.fruit.item)
		{
			var con = Target.fruit.item->GetCon(1000);
			if (Target.fruit.mesh)
			{
				Target->SetAttachTransform(Target.fruit.mesh, Trans_Scale(con, con, con));
			}
			
			if (con >= 1000)
			{
				Target.Collectible = true;
				return FX_Execute_Kill;
			}
			
			return FX_OK;
		}
		else
		{
			return FX_Execute_Kill;
		}
	},
};


local FxGrowFruit = new Effect
{
	Timer = func(int time)
	{
		Target->GrowFruit();
		return FX_Execute_Kill;
	},
};

/* -- Graphics -- */


private func TransformLeaf(string bone, int x, int y, int z)
{
	TransformBone(bone, Trans_Mul(Trans_Rotate(x, 1, 0, 0), Trans_Rotate(y, 0, 1, 0), Trans_Rotate(z, 0, 0, 1)), 1, Anim_Const(1000));
}

/* -- Damage -- */

public func CanBeHitByShockwaves() { return true; }
public func DoShockwaveCheck(int x, int y, int cause_plr)
{
	if (GetEffect("FxDestroy", this))
	{
		return _inherited(x, y, cause_plr);
	}
	else
	{
		return true;
	}
}

public func OnShockwaveHit()
{
	if (!GetEffect("FxDestroy", this))
	{
		CreateEffect(FxDestroy, 1, 1);
	}
	return false;
}

local FxDestroy = new Effect
{
	Construction = func ()
	{
		// make movable but invisible
		Target->SetCategory(C4D_Vehicle); 
		this.vis = Target.Visibility;
		Target->CreateParticle("Grass", 0, 0, PV_Random(-20, 20), PV_Random(-20, 10), PV_Random(30, 100), Particles_Straw(), 30);
		Target.Visibility = VIS_Editor;
		Target->SetObjectBlitMode(GFX_BLIT_Additive);

		// launch the fruit
		this.picked = Target->PickFruit();
		if (this.picked && this.picked->GetCon() < 100)
		{
			this.picked->RemoveObject();
		}
	},

	Timer = func (int time)
	{
		// launch previous bomb in same direction as myself
		if (time == 1)
		{
			if (this.picked)
			{
				var precision = 1000;
				this.picked->SetXDir(Target->GetXDir(precision), precision);
				this.picked->SetYDir(Target->GetYDir(precision), precision);
			}
			
			Target->SetXDir();
			Target->SetYDir();
		}
		
		// reset growth timer
		var grow = GetEffect("FxGrowFruit", Target);
		if (grow)
		{
			grow.Time = 0;
		}
		
		// check for movement
		if (!Target->GetXDir() && ! Target->GetYDir())
		{
			++this.no_motion;
		}
		
		if (this.no_motion >= 60)
		{
			Target.Visibility = this.vis;
			Target->SetCategory(C4D_StaticBack);
			Target->SetObjectBlitMode(nil);
			Target->FadeIn(60);
			return FX_Execute_Kill;
		}
	},	
};

/* -- Properties -- */

public func Place(int amount, proplist area, proplist settings)
{
	var placed = inherited(amount, area, settings);
	
	for (var plant in placed)
	{
		plant->RootSurface(30);
	}

	return placed;
}

local Collectible = true;
local Name = "$Name$";
local Description = "$Description$";
