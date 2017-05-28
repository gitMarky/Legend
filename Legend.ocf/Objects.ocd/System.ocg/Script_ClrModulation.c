/**
 Color modulation.
 
 Instead of using one color modulation, the object can be modulated by several channels.
 */


/**
 Adds color modulation info to the object.
 */
global func AddClrModulation(int color, int overlay, int ID)
{
	AssertObjectContext();
	
	// add the info
	if (this.ClrModulationInfo == nil)
	{
		this.ClrModulationInfo = [];
	}
	if (this.ClrModulationInfo[overlay] == nil)
	{
		this.ClrModulationInfo[overlay] = [];
	}
	
	var channel;
	if (ID == nil)
	{
		for (channel = 0; channel < GetLength(this.ClrModulationInfo[overlay]); ++channel)
		{
			if (this.ClrModulationInfo[overlay][channel] == nil)
			{
				break;
			}
		}
	}
	else
	{
		channel = ID;
	}
	
	this.ClrModulationInfo[overlay][channel] = color;
	
	UpdateClrModulation(overlay);

	return channel;
}


/**
 Removes a color modulation from an object.
 */
global func RemoveClrModulation(int ID, int overlay)
{
	AssertObjectContext();

	if (this.ClrModulationInfo && this.ClrModulationInfo[overlay])
	{
		this.ClrModulationInfo[overlay][ID] = nil;
	}
	
	UpdateClrModulation(overlay);
}


/**
 Calculates the final color modulation from all color modulations.
 */
global func UpdateClrModulation(int overlay)
{
	AssertObjectContext();
	
	if (this.ClrModulationInfo &&  this.ClrModulationInfo[overlay])
	{
		var max = 255;
		var r = 255;
		var g = 255;
		var b = 255;
		var a = 255;
		
		for (var i = 0; i < GetLength(this.ClrModulationInfo[overlay]); ++i)
		{
			var color = this.ClrModulationInfo[overlay][i];
			if (color == -1)
			{
				color = nil;
				// remove empty modulations from the array
				this.ClrModulationInfo[overlay][i] = nil;
			}
			
		
			if (color == nil) continue;
		
			var current_a = GetRGBaValue(color, RGBA_ALPHA);
			var current_r = GetRGBaValue(color, RGBA_RED);
			var current_g = GetRGBaValue(color, RGBA_GREEN);
			var current_b = GetRGBaValue(color, RGBA_BLUE);
			
			a = BoundBy(a * current_a / max, 0, max);
			r = BoundBy(r * current_r / max, 0, max);
			g = BoundBy(g * current_g / max, 0, max);
			b = BoundBy(b * current_b / max, 0, max);
		}
		
		SetClrModulation(RGBa(r, g, b, a), overlay);
	}
	else
	{
		SetClrModulation(RGBa(255, 255, 255, 255), overlay);
	}
}
