#include Library_CarryHeavy
#include Library_Plant

local fruit;

private func Construction()
{
	fruit = { item = nil, mesh = nil};
	GrowFruit(true);
	SetSeedChance(0);
	SetSeedAmount(0);
	
	// rotate
	this.MeshTransformation = Trans_Rotate(RandomX(0, 359), 0, 1, 0);
	
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

/* -- Graphics -- */


private func TransformLeaf(string bone, int x, int y, int z)
{
	TransformBone(bone, Trans_Mul(Trans_Rotate(x, 1, 0, 0), Trans_Rotate(y, 0, 1, 0), Trans_Rotate(z, 0, 0, 1)), 1, Anim_Const(1000));
}


/*-- Properties --*/

local Collectible = true;
local Name = "$Name$";
local Description = "$Description$";
