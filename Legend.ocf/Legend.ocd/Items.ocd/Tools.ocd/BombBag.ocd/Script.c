#include Library_HasExtraSlot

/*-- Properties --*/

public func Hit()
{
	Sound("Hits::GeneralHit?");
}

local Name = "$Name$";
local Description = "$Description$";
local Collectible = 1;

/*-- Collection of bombs --*/

public func RejectCollect(id def, object item)
{
	// Can contain only bombs
	if (!item->~IsBomb()) return true;
	// Can only contain one stackable object.
	if (Contents() && Contents(0)->~IsStackable()) return true;
	return false;
}

public func IsBombBag(){ return true; }
public func BombCapacity(){ return 30; }

local ExtraSlotFilter = "IsBomb"; // For editor-provided ammo list

/*-- Usage --*/

public func RejectUse(object user)
{
	return !user->HasHandAction(true, false, true);
}

public func ControlUse(object user, int x, int x)
{
	var bomb_stack = Contents();
	
	if (bomb_stack)
	{
		var bomb = bomb_stack->TakeObject();		
		user->Collect(bomb);
	}
	else
	{
		user->~PlaySoundDoubt();
	}

	return true;
}
