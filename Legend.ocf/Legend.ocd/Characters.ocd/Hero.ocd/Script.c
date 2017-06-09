#include Clonk
#include Library_DamageDisplay
#include Library_FallingDamage


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Properties

local MaxEnergy = 12000;

local FallSpeedFear = 100;


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


// Handle rolling when hitting the landscape
func HandleRollOnHit(int speed_x, int speed_y)
{
	if (this->IsWalking() && speed_y > 450)
	{
		// pressing forward?
		var x_movement = ComDir2XY(GetComDir())[0];
		var looking_right = GetDir() == DIR_Right;
		
		var request_roll = (x_movement > 0 && looking_right) || (x_movement < 0 && !looking_right); // you have to request rolling by pressing forward

		// determine collision angle
		var x_off = Abs(this->GetDefOffset(0)) + 1;
		var y_off = Abs(this->GetDefOffset(1)) / 2;
		var angle_fall = Angle(0, 0, speed_x, speed_y);
		var angle_landscape = GetSolidOffset(-x_off, y_off) - GetSolidOffset(x_off, y_off);
		
		var angle_roll = Normalize(angle_fall - angle_landscape, -180);
		if (!looking_right) angle_roll *= -1; // mirror
		
		var can_roll = Inside(angle_roll, 110, 165) && (speed_y < 10 * FallSpeedFear); // you can roll only if you are not, for example, bumping into a wall, or if you are not falling too straight
		//Log("Debug Rolling: angle_fall = %d, angle_landscape = %d, angle_roll = %d", angle_fall, angle_landscape, angle_roll);
		
		// determine final action
		
		if (can_roll && request_roll) // roll :D
		{
			DoRoll();
		}
		else // Force kneel-down when hitting the ground at high velocity.
		{
			DoKneel(true);
		}
	}
}

private func GetSolidOffset(int x, int y)
{
	var i;
	for (i = -15; GBackSolid(x, y - i) && (i < 15); ++i);
	return i;
}


public func StartJump()
{
	if (!GetEffect("IsFalling", this))
		CreateEffect(IsFalling, 1, 1);
		
	_inherited(...);
}


local IsFalling = new Effect
{
	Timer = func()
	{
		if (Target->GetAction() == "Jump")
		{
			if (Target->GetYDir() > Target.FallSpeedFear)
			{
				Target->PlaySoundScream();
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
