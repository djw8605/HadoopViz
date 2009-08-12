/*
 * SelectionDisplay.h
 *
 *  Created on: Jun 30, 2009
 *      Author: derekweitzel
 */

#ifndef SELECTIONDISPLAY_H_
#define SELECTIONDISPLAY_H_

#define _seldisp SelectionDisplay::GetInstance()

class SelectionDisplay {
public:
	SelectionDisplay();
	virtual ~SelectionDisplay();
	static SelectionDisplay* GetInstance();
	void SetText(char* text);
	void ClearText();
	void Render();

private:
	char* m_text;


};

#endif /* SELECTIONDISPLAY_H_ */
