
public func Damage(int change, int cause, int by_player)
{
	_inherited(change, cause, by_player);
	
	if (change > 0)
	{
		DamageFlash(change);
	}
}
