/*
 * UI.h
 *
 *  Created on: Jun 14, 2010
 *      Author: derekweitzel
 */

#ifndef UI_H_
#define UI_H_

#define _ui UI::GetInstance()

class UI
{
public:
	UI();
	virtual ~UI();
	static UI* GetInstance();
	void Render();



private:

	void Initialize2d();
	void Deinitialize2d();
	void RenderColorBar();

};

#endif /* UI_H_ */
