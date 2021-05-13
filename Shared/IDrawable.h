#pragma once

// Interface that ensures that an object can be drawn
class IDrawable
{
public:
	virtual void draw() const = 0;
};