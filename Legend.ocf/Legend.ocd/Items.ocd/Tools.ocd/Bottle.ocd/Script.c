/**
	Bottle
	Small container.

	@author Marky
*/


/*-- Engine Callbacks --*/

func Hit()
{
	Sound("Hits::Materials::Glass::GlassHit?");
}


func Hit2()
{
	// Cast some particles.
		// TODO?
	// Sound effects.
	Sound("Hits::Materials::Glass::GlassBreak");
	RemoveObject();
}

/* -- Usage -- */

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
