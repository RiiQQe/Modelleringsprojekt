/*
 *  Box.h
 *  Graphics Assignment Four
 *
 *  Created by Karl Grogan on 27/04/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

class Box
{
private:
	float boxX;
	float boxY;
	float rotAngle;	
	int lastFrameTime;
	
public:
	Box();
	void MoveBox();
	void RotateBox();
	void TranslateBox(int key);
	void DrawBox();
	
};