
global func IsProjectileTarget(object projectile, object shooter)
{
	if (projectile->GetID() == Item_GrapplerHook && projectile->~CanStickTo(this))
	{
		return true;
	}

	return inherited(projectile, shooter);
}
