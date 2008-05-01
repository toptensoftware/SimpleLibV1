//////////////////////////////////////////////////////////////////////////
// ToolBarCtrlEx.h - declaration of CToolBarCtrlEx class

#ifndef __TOOLBARCTRLEX_H
#define __TOOLBARCTRLEX_H


#ifdef __ATLWIN_H__
#ifdef __ATLCONTROLS_H__

namespace Simple
{

BOOL SIMPLEAPI LoadToolBar(HWND hWndToolBar, HINSTANCE hInst, LPCTSTR pszResource, COLORREF rgbTransparent, HIMAGELIST* phImageList);
BOOL SIMPLEAPI LoadToolBar(HWND hWndToolBar, HINSTANCE hInst, LPCTSTR pszResource, LPCTSTR pszResource32, COLORREF rgbTransparent, HIMAGELIST* phImageList);
void SIMPLEAPI SetToolBarButtonLabel(HWND hWndToolBar, UINT nID, LPCTSTR pszLabel);

class CToolBarCtrlEx : public ATLControls::CToolBarCtrl
{
public:
	CToolBarCtrlEx()
	{
		m_hImageList=NULL;
	}
	virtual ~CToolBarCtrlEx()
	{
		if (m_hImageList)
			ImageList_Destroy(m_hImageList);
	}

	void LoadToolBar(UINT nResID, UINT nResID32=0, COLORREF rgbMask=RGB(192,192,192))
	{
		if (m_hImageList)
			{
			ImageList_Destroy(m_hImageList);
			m_hImageList=NULL;
			}

		Simple::LoadToolBar(m_hWnd, _AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(nResID), MAKEINTRESOURCE(nResID32), rgbMask, &m_hImageList);
	}

	void AddButton(UINT nID, UINT nImage)
	{
		TBBUTTON button;
		button.iBitmap=nImage;
		button.idCommand=nID;
		button.fsState=TBSTATE_ENABLED;
		button.fsStyle=TBSTYLE_BUTTON;
		button.dwData=0;
		button.iString=0;
		SendMessage(TB_ADDBUTTONS, 1, (LPARAM)&button);
	}

	void SetButtonLabel(UINT nID, LPCTSTR pszLabel)
	{
		SetToolBarButtonLabel(m_hWnd, nID, pszLabel);
	}

	void SetButtonImage(UINT nID, int iImage)
	{
		// Set button text
		TBBUTTONINFO info;
		memset(&info, 0, sizeof(info));
		info.cbSize=sizeof(info);
		info.dwMask=TBIF_IMAGE;
		info.iImage=iImage;
		SetButtonInfo(nID, &info);
	}

	HIMAGELIST	m_hImageList;
};


}	// namespace Simple

#endif	//	__ATLCONTROLS_H__
#endif	//	__ATLWIN_H__

#endif	// __TOOLBARCTRLEX_H

