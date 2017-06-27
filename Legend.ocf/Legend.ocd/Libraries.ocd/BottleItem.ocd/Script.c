/*
	Bottle item
	
	Item that can be carried inside a bottle
*/


public func IsBottleItem() { return true; }

local bottle_mesh;

public func Construction()
{
	_inherited();
	
	if (HasBottleMesh())
	{
		bottle_mesh = CreateObject(Dummy);
		bottle_mesh->SetPosition(0, 0);
		bottle_mesh->SetObjectLayer(bottle_mesh);
		bottle_mesh->SetGraphics(nil, HasBottleMesh());
	}
}


public func Destruction()
{
	_inherited();
	
	// this is intentioannly not GetBottleMesh(), so that GetBottleMesh() can return the object itself.
	if (bottle_mesh) bottle_mesh->RemoveObject();
}


public func HasBottleMesh() { return nil; } // returns an ID for an external mesh, if the object for example, has sprite graphics usually
public func GetBottleMesh() { return bottle_mesh; }
