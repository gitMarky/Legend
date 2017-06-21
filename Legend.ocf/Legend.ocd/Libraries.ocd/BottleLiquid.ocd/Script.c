/*
	Bottle liquid
	
	Liquid that can be carried inside a bottle
*/

#include Library_BottleItem


public func HasBottleMesh() { return Library_BottleItemMesh; }

public func UpdateBottleMesh(object bottle, int attach_number)
{
	var fill_level = BoundBy(1000 * this->GetStackCount() / this->MaxStackCount(), 1, 1000);	
	var offset = (1600 * (1001 - fill_level)) / fill_level;

	bottle->SetAttachTransform(attach_number, Trans_Mul(Trans_Scale(1000, fill_level, 1000), Trans_Translate(0, offset, 0)));

	var color = GetAverageTextureColor(this->GetLiquidMaterial());
	color = SetRGBaValue(color, 100, RGBA_ALPHA);

	GetBottleMesh()->SetClrModulation(color);
}
