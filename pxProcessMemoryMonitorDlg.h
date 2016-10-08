
// pxProcessMemoryMonitorDlg.h : ͷ�ļ�
//

#pragma once


// CpxProcessMemoryMonitorDlg �Ի���
class CpxProcessMemoryMonitorDlg : public CDialogEx
{
// ����
public:
	CpxProcessMemoryMonitorDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_pxProcessMemoryMonitor_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
	CString m_strProcessName;
	CString m_strMonitorState;
};
