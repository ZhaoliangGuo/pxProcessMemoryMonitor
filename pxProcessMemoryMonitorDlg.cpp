
// pxProcessMemoryMonitorDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "pxProcessMemoryMonitor.h"
#include "pxProcessMemoryMonitorDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD WINAPI ThreadStartMonitor(LPVOID pParam);

#include "pxMemoryManager.h"
#include "pxCommonDef.h"

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CpxProcessMemoryMonitorDlg �Ի���




CpxProcessMemoryMonitorDlg::CpxProcessMemoryMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CpxProcessMemoryMonitorDlg::IDD, pParent)
	, m_strProcessName(_T(""))
	, m_strMonitorState(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CpxProcessMemoryMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strProcessName);
	DDX_Text(pDX, IDC_STATIC_MONITOR_STATE, m_strMonitorState);
}

BEGIN_MESSAGE_MAP(CpxProcessMemoryMonitorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START_MONITOR, &CpxProcessMemoryMonitorDlg::OnBnClickedButtonStartMonitor)
	ON_BN_CLICKED(IDC_BUTTON_STOP_MONITOR,  &CpxProcessMemoryMonitorDlg::OnBnClickedButtonStopMonitor)
	ON_BN_CLICKED(IDC_BUTTON_UNIT_TEST,     &CpxProcessMemoryMonitorDlg::OnBnClickedButtonTest)
	ON_MESSAGE(WM_UPDATE_MEMORY_VALUE,      &CpxProcessMemoryMonitorDlg::UpdateMemoryValue)
END_MESSAGE_MAP()


// CpxProcessMemoryMonitorDlg ��Ϣ�������

BOOL CpxProcessMemoryMonitorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

		// ��ȡ������ľ��
	CWnd *pWnd  = AfxGetMainWnd();
	pWnd->GetWindowText(g_strAppTitle);
	g_hAppWnd = ::FindWindow(NULL, g_strAppTitle);

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	Init();

	OnBnClickedButtonStartMonitor();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CpxProcessMemoryMonitorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CpxProcessMemoryMonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CpxProcessMemoryMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CpxProcessMemoryMonitorDlg::OnBnClickedButtonStartMonitor()
{
	UpdateData();

	GetDlgItem(IDC_EDIT_PROCESS_NAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_START_MONITOR)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP_MONITOR)->EnableWindow(TRUE);

	SaveConfig(); // ����������������ļ�

	AfxBeginThread((AFX_THREADPROC)ThreadStartMonitor,
		this,
		THREAD_PRIORITY_NORMAL);

	m_strMonitorState = "�����...";

	UpdateData(FALSE);
}

bool g_bStop = false; // ���ڿ��ƿ�ʼ�ͽ���

DWORD WINAPI ThreadStartMonitor(LPVOID pParam)
{
	g_bStop = false;

	CpxProcessMemoryMonitorDlg *pCpxProcessMemoryMonitorDlg = (CpxProcessMemoryMonitorDlg *)pParam;

	double dProcessCommitMemSize = 0;
	double dProcessWorkingSet    = 0;

	char *pszProcessName = (char *)pCpxProcessMemoryMonitorDlg->m_strProcessName.GetBuffer(pCpxProcessMemoryMonitorDlg->m_strProcessName.GetLength());

	char szLogFileName[_MAX_PATH] = {0};

	PROCESS_MEMORY_COUNTERS pmc;

	while (!g_bStop)
	{	
		SYSTEMTIME   time;
		GetLocalTime(&time);
		sprintf_s(szLogFileName,
			_MAX_PATH, 
			".\\MemoryLogFile-%s-%d.%d.%d.txt",
			pszProcessName,
			time.wYear, 
			time.wMonth, 
			time.wDay);

		memset(&pmc, 0, sizeof(PROCESS_MEMORY_COUNTERS));
		pmc =  GetProcessMemoryCounters(pszProcessName);

		// �ύ�ڴ��С
		//pmc.PagefileUsage = 16 * 1024 * 1024 * 1024; // test  
		dProcessCommitMemSize = pmc.PagefileUsage  * 1.0/(1024*1024);
		dProcessWorkingSet    = pmc.WorkingSetSize * 1.0/(1024*1024);

		//dProcessCommitMemSize = (static_cast<unsigned long>(pmc.PagefileUsage)) * 1.0/(1024*1024);
		//dProcessWorkingSet    = (static_cast<unsigned long>(pmc.WorkingSetSize)) * 1.0/(1024*1024);
			
		FILE *fp = fopen(szLogFileName, "at");
		fprintf(fp, 
			"%4d-%02d-%02d %02d:%02d:%02d.%03d  Commit Memory: %5.2fM; Current Working Set: %5.2fM\n",
			time.wYear,
			time.wMonth,
			time.wDay,
			time.wHour,
			time.wMinute, 
			time.wSecond,
			time.wMilliseconds,
			dProcessCommitMemSize,
			dProcessWorkingSet
			);

		fflush(fp);
		fclose(fp);

		g_strMsg.Format("Commit Memory: %5.2fM; Current Working Set: %5.2fM",
			dProcessCommitMemSize,
			dProcessWorkingSet);

		::PostMessage(g_hAppWnd, WM_UPDATE_MEMORY_VALUE, NULL, (LPARAM)g_strMsg.GetBuffer());

		Sleep(500);
	}

	return 0;
}

void CpxProcessMemoryMonitorDlg::OnBnClickedButtonStopMonitor()
{
	UpdateData();

	g_bStop = true;

	m_strMonitorState = "���ֹͣ.";

	//ShellExecuteA(NULL, "open", ".\\", NULL, NULL, SW_SHOW);

	GetDlgItem(IDC_EDIT_PROCESS_NAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_START_MONITOR)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP_MONITOR)->EnableWindow(FALSE);

	UpdateData(FALSE);
}

// ��Ԫ����
void CpxProcessMemoryMonitorDlg::OnBnClickedButtonTest()
{
	UpdateData();

	// test GetProcessIdByNameEx
	//char *pszProcessName = "pxMediaServerApp.exe";

	char *pszProcessName = (char *)m_strProcessName.GetBuffer(m_strProcessName.GetLength());

	DWORD dwProcessId = GetProcessIdByName(pszProcessName);

	CString strProcessId("");
	strProcessId.Format("%u", dwProcessId);

	//AfxMessageBox(strProcessId);

	// test GetProcessMemorySize
	double dProcessMemSize = GetProcessCommitMemorySize(pszProcessName);

	CString strProcessMemSize("");
	strProcessMemSize.Format("%.2f", dProcessMemSize);

	CString strMsg("");

	strMsg += "strProcessName: ";
	strMsg += m_strProcessName;
	strMsg += ";\n strProcessId: ";
	strMsg += strProcessId;
	strMsg += ";\n strProcessMemSize: ";
	strMsg += strProcessMemSize;
	strMsg += "M.";

	//AfxMessageBox(strMsg);

	PROCESS_MEMORY_COUNTERS pmc;
	memset(&pmc, 0, sizeof(PROCESS_MEMORY_COUNTERS));
	pmc =  GetProcessMemoryCounters(pszProcessName);
}

void CpxProcessMemoryMonitorDlg::Init()
{
	UpdateData();

	// ��ini�ļ��ж�ȡProcessName

	//m_strProcessName = "pxMediaServerApp.exe";
	//m_strProcessName = "taskmgr.exe";

	char szProcessName[_MAX_PATH] = {0};
	GetPrivateProfileString("Monitor", "ProcessName", "pxMediaServerApp.exe", szProcessName, sizeof(szProcessName), g_strConfFile);
	m_strProcessName.Format("%s", szProcessName);

	m_strMonitorState = "�����ʼ��ذ�ť���ɿ�ʼ���ָ������...";

	UpdateData(FALSE);
}

void CpxProcessMemoryMonitorDlg::SaveConfig()
{
	UpdateData();

	WritePrivateProfileString ("Monitor", "ProcessName",   m_strProcessName,  g_strConfFile);

	UpdateData(FALSE);
}

LRESULT CpxProcessMemoryMonitorDlg::UpdateMemoryValue( WPARAM wParam, LPARAM lParam )
{
	UpdateData();

	char *pszMsg = (char *)lParam;
	CString strMsg;

	try
	{
		strMsg.Format("%s", pszMsg);
	}
	catch (CException* e)
	{
		TCHAR szError[128] = {0};
		e->GetErrorMessage(szError, 128);
		e->Delete();
		g_strMsg.Format("%s", szError);

		return -1;
	}

	if (strMsg == "")
	{
		return -1;
	}

	m_strMonitorState = strMsg;

	UpdateData(FALSE);

	return 0;
}

