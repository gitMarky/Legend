
func Hit(int speed_x, int speed_y)
{
	CreateEffect(FxFallingDamage, 1, 5, speed_x / 10, speed_y / 10);
	_inherited(speed_x, speed_y, ...);
}


func DoRoll()
{
	// rolling reduces falling damage
	var falling_damage = GetEffect("FxFallingDamage", this);
	
	if (falling_damage)
	{
		falling_damage.remaining_damage /= 2;
	}

	_inherited(...);
}


local FxFallingDamage = new Effect
{
	Construction = func (int speed_x, int speed_y)
	{
		var velocity = Distance(speed_x, speed_y);
		this.total_damage = Max(0, velocity / 4 - 12);

		if (this.total_damage > 0)
		{
			this.remaining_damage = this.total_damage;
			this.Target->~CatchBlow();
		}
	},
	
	Timer = func ()
	{
		if (this.remaining_damage > 0)
		{
			var damage = Min(this.remaining_damage, 2);
			this.remaining_damage -= damage;	
			
			this.Target->DoDamage(damage);

			if (this.Target->GetAlive())
				this.Target->DoEnergy(-damage);

			return FX_OK;
		}
		else
		{
			return FX_Execute_Kill;
		}
	},
};
