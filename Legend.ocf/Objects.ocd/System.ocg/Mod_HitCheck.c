
global func IsProjectileTarget(object projectile, object shooter)
{
	if (projectile->GetID() == Item_GrapplerHook && (this.Collectible || this->~IsHookLauncherTarget()))
	{
		return true;
	}

	return inherited(projectile, shooter);
}
