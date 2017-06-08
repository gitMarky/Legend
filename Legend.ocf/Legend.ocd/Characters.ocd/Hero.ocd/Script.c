#include Clonk
#include Library_DamageDisplay


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Properties

local MaxEnergy = 12000;


func SetSkin(int new_skin)
{
	RemoveBackpack(); // remove the backpack

	SetGraphics(nil, Clonk);

	// Go back to original action afterwards and hope
	// that noone calls SetSkin during more complex activities
	var prev_action = GetAction();
	SetAction("Jump");
	SetAction(prev_action);
	SetOwner(GetOwner()); // somehow the color did not get updated after setting the color

	return skin;
}
