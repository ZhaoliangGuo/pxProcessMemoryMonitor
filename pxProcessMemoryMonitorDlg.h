
// pxProcessMemoryMonitorDlg.h : 头文件
//

#pragma once


// CpxProcessMemoryMonitorDlg 对话框
class CpxProcessMemoryMonitorDlg : public CDialogEx
{
// 构造
public:
	CpxProcessMemoryMonitorDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_pxProcessMemoryMonitor_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStartMonitor();
	afx_msg void OnBnClickedButtonStopMonitor();
	afx_msg void OnBnClickedButtonTest();
	void Init();
	void SaveConfig();
	LRESULT UpdateMemoryValue( WPARAM wParam, LPARAM lParam );
	CString m_strProcessName;
	CString m_strMonitorState;

public:
	double m_dLastProcessCommitMemSizeInKB;
	double m_dLastProcessWorkingSetInKB;   
};
