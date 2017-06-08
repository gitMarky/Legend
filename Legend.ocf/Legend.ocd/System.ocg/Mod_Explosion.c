
global func BlastObject(int level, int caused_by)
{
	var self = this;
	
	_inherited(level, caused_by);
	
	if (!self) return;
	this->~CatchBlow();	
	return;
}