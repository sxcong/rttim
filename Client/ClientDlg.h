// ClientDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "VideoWnd.h"
#include "CameraDS.h"
#include "VideoDec.h"
#include "VideoEnc.h"
#include "UDPSession.h"

// CClientDlg 对话框
class CClientDlg : public CDialog
{
// 构造
public:
	CClientDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CClientDlg();

// 对话框数据
	enum { IDD = IDD_CLIENT_DIALOG };

	static void OnVideoCap(int nID, BYTE* buff, long len, void* lpParam);
	void ProcessVCapData(int nID, BYTE* buff, long len);


	static void	VDecodeCB(BYTE* pData, UINT nLen, void* lpParam);
	void ProcessDecodeData(BYTE* pData, UINT nLen);

	static void	VEncodeCB(BYTE* pData, UINT nLen, void* lpParam);
	void ProcessEncodeData(BYTE* pData, UINT nLen);	

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	CVideoWnd* m_pLocal;
	CVideoWnd* m_pRemote;

	CVideoEnc m_enc;
	CVideoDec m_dec;

	CCameraDS m_CamDS;

	CUDPSession m_Sess;
	int m_nSessID;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CComboBox m_comboCam;
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonClose();

	static void OnUDPDataCB(int sockid, char *data, int len, int ip, int port, void* param);
	void OnRecvFrom(int sockid, char *data, int len, int ip, int port);
};
