/**
	Bottle
	Small container.

	@author Marky
*/

#include Library_MeleeWeapon
#include Library_LiquidContainer
#include Library_HasExtraSlot


/*-- Engine Callbacks --*/

func Hit()
{
	Sound("Hits::Materials::Glass::GlassHit?");
}


/* -- Usage -- */

public func RejectUse(object clonk)
{
	return !(clonk->HasHandAction() && CanStrikeWithWeapon(clonk) && (clonk->IsWalking() || clonk->IsJumping()));
}

public func ControlUse(object clonk, int x, int y)
{
	if (IsEmpty())
	{
		StartCollection(clonk);
		return true;
	}
	else
	{
		var item = Contents();
		
		if (item.ControlUse && item->ControlUse(clonk, x, y))
		{
			return true;
		}
		else if (item->~IsLiquid())
		{
			item->~Disperse(Angle(0, 0, x, y), 10);
			return true;
		}
		else
		{
			item->Exit(7 * clonk->GetCalcDir());
			return true;
		}
	}
}


/* -- Collection -- */

public func RejectCollect(id type, object item)
{
	if (item->~IsBottleItem() || CanCollectLiquid(item))
	{
		// Can only contain one object
		return Contents();
	}
	else // Reject everything else
	{
		return true;
	}
}


public func CollectFromStack(object item)
{
	// Callback from stackable object: Try grabbing partial objects from this stack, if the stack is too large
	if (item->GetStackCount() > GetLiquidAmountRemaining() && !this->RejectStack(item))
	{
		// Get one sample object and try to insert it into the bottle
		var candidate = item->TakeObject();
		candidate->Enter(this);

		// Put it back if it was not collected
		if (candidate && !(candidate->Contained()))
		{
			item->TryAddToStack(candidate);
		}
	}
}


public func RejectStack(object item)
{
	// Callback from stackable object: When should a stack entrance be rejected, if the object was not merged into the existing stacks?
	if (Contents())
	{
		// The bottle can hold only one type of liquid
		return true;
	}
	if (CanCollectLiquid(item))
	{
		// The liquid is suitable, collect it!
		return false;
	}
	else
	{
		// Reject anything else
		return true;
	}
}


public func GetLiquidContainerMaxFillLevel(liquid_name) { return 50; }


public func IsLiquidContainerForMaterial(string liquid_name)
{
	return !!WildcardMatch("Water", liquid_name) || !!WildcardMatch("Oil", liquid_name);
}


public func CanCollectLiquid(object item)
{
	return item->~IsLiquid() && this->IsLiquidContainerForMaterial(item->~GetLiquidType());
}


public func Collection2(object item)
{
	UpdateBottleContents();
	return _inherited(item, ...);
}


public func Ejection(object item)
{
	UpdateBottleContents();
	return _inherited(item, ...);
}


public func RemoveLiquid(liquid_name, int amount, object destination)
{
	var result = _inherited(liquid_name, amount, destination, ...);
	UpdateBottleContents();
	return result;
}


public func PutLiquid(liquid_name, int amount, object source)
{
	var result = _inherited(liquid_name, amount, source, ...);
	UpdateBottleContents();
	return result;
}


/* -- Internals -- */

private func StartCollection(object clonk) // play collection animation
{
	var arm = "R";
	if (clonk->GetHandPosByItemPos(clonk->GetItemPos(this)) == 1)
	{
		arm = "L";
	}

	var animation = Format("BottleSwing.%s", arm);
	var animlen = clonk->GetAnimationLength(animation);
	var action_length = 20;

	if (!GetEffect("BottleWeaponCooldown", clonk))
	{
		AddEffect("BottleWeaponCooldown", clonk, 2, action_length, this);
	}

	PlayWeaponAnimation(clonk, animation, 10, Anim_Linear(0, 0, animlen, action_length, ANIM_Remove), Anim_Linear(0, 0, 1000, 5, ANIM_Remove));
	clonk->UpdateAttach();
	StartWeaponHitCheckEffect(clonk, action_length, 1);

	this->Sound("Objects::Weapons::WeaponSwing?");
}


private func CheckStrike(int time) // try to collect items while the animation is running
{
	if (time < 10) return;

	var offset_x = 7 * Contained()->GetCalcDir();
	var offset_y = 5;

	var width = 10;
	var height = 20;
	
	if (IsEmpty())
	{
		for (var item in FindObjects(Find_AtRect(offset_x - width/2, offset_y - height/2, width, height),
								     Find_NoContainer(),
								     Find_Exclude(Contained()),
								     Find_Func("IsBottleItem"),
								     Find_Layer(GetObjectLayer())))
		{
			Collect(item, true);

			if (!IsEmpty())
			{
				break;
			}
		}
	}
}


private func WeaponStrikeExpired()
{
	if (GetEffect("BottleWeaponCooldown", Contained()))
		RemoveEffect("BottleWeaponCooldown", Contained());
}


private func IsEmpty()
{
	return !Contents();
}


/* -- Display -- */

public func GetCarryBone()
{
	return "Base";
}

public func GetCarryMode()
{
	return CARRY_Hand;
}


public func GetCarryTransform(object clonk, bool idle, bool nohand)
{
	if (nohand)
	{
		return Trans_Scale(1, 1, 1);
	}
	else
	{
		return Trans_Mul(Trans_Rotate(90, 0, 0, 1), Trans_Translate(1000, 2000, 0));
	}
}


private func Definition(proplist def)
{
	def.PictureTransformation = Trans_Mul(Trans_Rotate(280, 0, 1, 0), Trans_Rotate(20, 0, 0, 1), Trans_Rotate(5, 1, 0, 0), Trans_Translate(0, 0, 250));
}


private func UpdateBottleContents()
{
	if (Contents())
	{
		var mesh = Contents()->GetBottleMesh();
		if (mesh)
		{
			if (!bottle_attach_number)
			{
				bottle_attach_number = AttachMesh(mesh, this->GetCarryBone(), mesh->~GetBottleBone() ?? "Base", Trans_Identity());
			}
	
			Contents()->UpdateBottleMesh(this, bottle_attach_number);
		}
	}
	else
	{
		if (bottle_attach_number)
		{
			DetachMesh(bottle_attach_number);
			bottle_attach_number = nil;
		}
	}
}

/* -- Properties -- */


local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;

local bottle_attach_number; // attached mesh
