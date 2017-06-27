#include Library_BottleItem
#appendto Butterfly

public func GetBottleMesh() { return this; }   // the butterfly can attach itself to the bottle
public func GetBottleBone() { return "main"; } // attach bone

public func UpdateBottleMesh(object bottle, int attach_number)
{
	bottle->SetAttachTransform(attach_number, Trans_Rotate(270, 0, 1, 1));
}
