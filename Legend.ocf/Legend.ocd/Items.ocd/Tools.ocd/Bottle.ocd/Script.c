/**
	Bottle
	Small container.

	@author Marky
*/

#include Library_LiquidContainer
#include Library_HasExtraSlot


/*-- Engine Callbacks --*/

func Hit()
{
	Sound("Hits::Materials::Glass::GlassHit?");
}


/* -- Usage -- */

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

/* -- Properties -- */


local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
