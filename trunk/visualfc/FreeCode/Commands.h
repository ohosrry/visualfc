// Commands.h : header file
//

#if !defined(AFX_COMMANDS_H__48B9E21E_C60B_488B_B569_E7D1ED94E944__INCLUDED_)
#define AFX_COMMANDS_H__48B9E21E_C60B_488B_B569_E7D1ED94E944__INCLUDED_

#include "FreeCodeTypes.h"

#include <atlapp.h>
#include <atlwin.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atlsplit.h>
#include <atlmisc.h>

#include "../public/fc_core.h"
#include "./dspparser.h"

#ifdef _DEBUG
	#ifndef ASSERT
		#define ASSERT(x) ATLASSERT(x)
	#endif
#endif //_DEBUG

#ifndef VERIFY
	#define VERIFY(x) ATLVERIFY(x)
#endif

class CCommands : 
	public CComDualImpl<ICommands, &IID_ICommands, &LIBID_FreeCode>, 
	public CComObjectRoot,
	public CComCoClass<CCommands, &CLSID_Commands>,
	public fox::vs60_addin
{
protected:
	IApplication* m_pApplication;
	fc_core	   m_fc;
	dspparser  m_dsp;
public:
	CCommands();
	~CCommands();
	virtual int get_line_column(const char * file, int line);
	virtual bool insert_block(const char * file, int line, int column, const char * text);
	virtual bool replace_text(const char * file, int line, const char * old, const char * text);
	virtual bool goto_line(const char * file, int line);
	virtual size_t get_file_count(const char * project_name)
	{
		return m_dsp.source.GetSize();
	}
	virtual const char * get_files(const char * project_name, int i)
	{
		return (LPCTSTR)m_dsp.source[i];
	}
	
	LRESULT GetOpenFileDocument(CComPtr<IDispatch> & Document, const char * file);
	LRESULT SaveAllDocuments();
	LRESULT InsertDocumentText(CComPtr<IDispatch> & Document, int line, int column, const char * text);
	LRESULT ReplaceDocumentText(CComPtr<IDispatch> & Document, int line, const char * old, const char * text);


	void SetApplicationObject(IApplication* m_pApplication);
	IApplication* GetApplicationObject() { return m_pApplication; }
	void UnadviseFromEvents();

	BEGIN_COM_MAP(CCommands)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(ICommands)
	END_COM_MAP()
	DECLARE_NOT_AGGREGATABLE(CCommands)

protected:
	// This class template is used as the base class for the Application
	//  event handler object and the Debugger event handler object,
	//  which are declared below.
	template <class IEvents, const IID* piidEvents, const GUID* plibid,
		class XEvents, const CLSID* pClsidEvents>
	class XEventHandler :
		public CComDualImpl<IEvents, piidEvents, plibid>,
		public CComObjectRoot,
		public CComCoClass<XEvents, pClsidEvents>
	{
	public:
		BEGIN_COM_MAP(XEvents)
			COM_INTERFACE_ENTRY(IDispatch)
			COM_INTERFACE_ENTRY_IID(*piidEvents, IEvents)
		END_COM_MAP()
		DECLARE_NOT_AGGREGATABLE(XEvents)
		void Connect(IUnknown* pUnk)
		{ VERIFY(SUCCEEDED(AtlAdvise(pUnk, this, *piidEvents, &m_dwAdvise))); }
		void Disconnect(IUnknown* pUnk)
		{ AtlUnadvise(pUnk, *piidEvents, m_dwAdvise); }

		CCommands* m_pCommands;

	protected:
		DWORD m_dwAdvise;
	};

	// This object handles events fired by the Application object
	class XApplicationEvents : public XEventHandler<IApplicationEvents, 
		&IID_IApplicationEvents, &LIBID_FreeCode, 
		XApplicationEvents, &CLSID_ApplicationEvents>
	{
	public:
		// IApplicationEvents methods
		STDMETHOD(BeforeBuildStart)(THIS);
		STDMETHOD(BuildFinish)(THIS_ long nNumErrors, long nNumWarnings);
		STDMETHOD(BeforeApplicationShutDown)(THIS);
		STDMETHOD(DocumentOpen)(THIS_ IDispatch * theDocument);
		STDMETHOD(BeforeDocumentClose)(THIS_ IDispatch * theDocument);
		STDMETHOD(DocumentSave)(THIS_ IDispatch * theDocument);
		STDMETHOD(NewDocument)(THIS_ IDispatch * theDocument);
		STDMETHOD(WindowActivate)(THIS_ IDispatch * theWindow);
		STDMETHOD(WindowDeactivate)(THIS_ IDispatch * theWindow);
		STDMETHOD(WorkspaceOpen)(THIS);
		STDMETHOD(WorkspaceClose)(THIS);
		STDMETHOD(NewWorkspace)(THIS);
	};
	typedef CComObject<XApplicationEvents> XApplicationEventsObj;
	XApplicationEventsObj* m_pApplicationEventsObj;

	// This object handles events fired by the Application object
	class XDebuggerEvents : public XEventHandler<IDebuggerEvents, 
		&IID_IDebuggerEvents, &LIBID_FreeCode, 
		XDebuggerEvents, &CLSID_DebuggerEvents>
	{
	public:
		// IDebuggerEvents method
	    STDMETHOD(BreakpointHit)(THIS_ IDispatch * pBreakpoint);
	};
	typedef CComObject<XDebuggerEvents> XDebuggerEventsObj;
	XDebuggerEventsObj* m_pDebuggerEventsObj;

public:
	STDMETHOD(Options)();
	STDMETHOD(WINXWizard)();
	STDMETHOD(WTLWizard)();
// ICommands methods
};

typedef CComObject<CCommands> CCommandsObj;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMANDS_H__48B9E21E_C60B_488B_B569_E7D1ED94E944__INCLUDED)
