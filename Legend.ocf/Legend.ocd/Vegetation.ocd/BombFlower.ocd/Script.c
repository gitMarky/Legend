#include Library_CarryHeavy
#include Library_Plant

local fruit;

private func Construction()
{
	fruit = { item = nil, mesh = nil};
	GrowFruit(true);
	SetSeedChance(0);
	SetSeedAmount(0);
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
	AssertNotNil(user);

	if (fruit.mesh)
	{
		DetachMesh(fruit.mesh);
		fruit.mesh = nil;
	}

	if (fruit.item)
	{
		fruit.item->Exit();
		user->Collect(fruit.item);
		
		fruit.item = nil;
		this.Collectible = false; // collectible only while there is fruit
	}
	
	RemoveEffect("FxGrowFruit", this);
	CreateEffect(FxGrowFruit, 1, 200);
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
	Timer = func()
	{
		Target->GrowFruit();
		return FX_Execute_Kill;
	},
};


/*-- Properties --*/

local Collectible = true;
local Name = "$Name$";
local Description = "$Description$";
