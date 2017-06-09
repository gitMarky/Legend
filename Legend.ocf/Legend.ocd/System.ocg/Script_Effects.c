
static const FxFlash = new Effect
{
	Construction = func(int color, int duration, int overlay)
	{
		color = color ?? RGBa(255, 255, 255, 255);
		this.a = GetRGBaValue(color, RGBA_ALPHA);
		this.r = GetRGBaValue(color, RGBA_RED);
		this.g = GetRGBaValue(color, RGBA_GREEN);
		this.b = GetRGBaValue(color, RGBA_BLUE);
		this.duration = duration ?? 36;
		this.overlay = overlay ?? 0;
	},

	Timer = func(int time)
	{
		var max = 255;
		var angle = BoundBy(time * 180 / this.duration, 0, 180);
		
		var scale = Sin(angle, max);
		
		var scale_invert = max - scale;
		
		var r = scale_invert + ((scale * this.r) / max);
		var g = scale_invert + ((scale * this.g) / max);
		var b = scale_invert + ((scale * this.b) / max);
		var a = scale_invert + ((scale * this.a) / max);
		var color = RGBa(r, g, b, a);

		this.channel = this.Target->AddClrModulation(color, this.overlay, this.channel);
	
		if (time >= this.duration)
		{
			return FX_Execute_Kill;
		}
		else
		{
			return FX_OK;
		}
	},

	Destruction = func()
	{
		if (this.channel)
			this.Target->RemoveClrModulation(this.channel, this.overlay);
	}
};


static const FxDamageFlash = new Effect
{
	Construction = func(int overlay)
	{
		this.overlay = overlay ?? 0;
	},

	Timer = func(int time)
	{
		var interval = 15;
		if (time % interval == 1)
		{
			this.Target->Flash(RGB(255, 170, 0), 10);
		}
		
		if (time >= BoundBy(15 * this.Amount, 15, 120))
		{
			return FX_Execute_Kill;
		}
		else
		{
			return FX_OK;
		}
	},
};


global func Flash(int color, int duration, int overlay)
{
	AssertObjectContext();
	CreateEffect(FxFlash, 1, 1, color, duration, overlay);
}


global func DamageFlash(int amount)
{
	AssertObjectContext();
	
	var damage = GetEffect("FxDamageFlash", this) ?? CreateEffect(FxDamageFlash, 1, 1);
	damage.Time = 0;
	damage.Amount = Max(damage.Amount, amount ?? 1);
}

