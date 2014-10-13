// ClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CClientDlg 对话框


 
int InitSockets()    
{    
#ifdef WIN32    
    WORD version;    
    WSADATA wsaData;    
    version = MAKEWORD(1, 1);    
    return (WSAStartup(version, &wsaData) == 0);    
#else    
    return TRUE;    
#endif    
}    
  
void CleanupSockets()    
{    
#ifdef WIN32    
    WSACleanup();    
#endif    
}    
  

CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pLocal = NULL;
	m_pRemote = NULL;
	m_nSessID = -1;
	InitSockets();
}

CClientDlg::~CClientDlg()
{
	SAFE_DELETE(m_pLocal);
	SAFE_DELETE(m_pRemote);
	CleanupSockets();
}



void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_VDEVICE, m_comboCam);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CClientDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CClientDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CClientDlg::OnBnClickedButtonClose)
END_MESSAGE_MAP()


// CClientDlg 消息处理程序

BOOL CClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	int m_iCamCount = CCameraDS::CameraCount();
	printf("There are %d cameras.\n", m_iCamCount);

	if(m_iCamCount == 0)
	{
		//return (-1);
		m_comboCam.InsertString(0, "No Camera");
	}
	else
	{
		//获取所有摄像头的名称
		for(int i = 0; i < m_iCamCount; i++)
		{
			char szCamName[1024];

			int retval = m_CamDS.CameraName(i, szCamName, sizeof(szCamName));

			if(retval >0)
			{
				printf("Camera #%d's Name is '%s'.\n", i, szCamName);
				m_comboCam.InsertString(i, szCamName);
			}
			else
			{
				printf("Can not get Camera #%d's name.\n", i);
			}
		}
	}

	m_comboCam.SetCurSel(0);

	MoveWindow(0, 0, 600, 520);

	m_pLocal = new CVideoWnd(this);
	m_pLocal->Create(IDD_VIDEO, CWnd::FromHandle(m_hWnd));
	m_pLocal->ShowWindow(SW_SHOW);

	m_pLocal->MoveWindow(0, 0, 320, 240);

	m_pRemote = new CVideoWnd(this);
	m_pRemote->Create(IDD_VIDEO, CWnd::FromHandle(m_hWnd));
	m_pRemote->ShowWindow(SW_SHOW);

	m_pRemote->MoveWindow(0, 250, 320, 240);
	

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CClientDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

BITMAPINFOHEADER g_bih;
FILE* pf;
void CClientDlg::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	//pf = fopen("c:\\rec.264", "wb");

	m_nSessID = m_Sess.Open(5500, OnUDPDataCB, this);
	if (m_nSessID < 0)
	{
		AfxMessageBox("open 5500 failed!");
		return;
	}

	m_enc.Enc_Init(320, 240, 20, 400000, VEncodeCB, this);
	m_dec.Dec_Init(320, 240, 20, 400000, VDecodeCB, this);


	g_bih.biSize = sizeof(BITMAPINFOHEADER);
	g_bih.biWidth = 320;
	g_bih.biHeight = 240;
	g_bih.biBitCount = 24;
	g_bih.biSizeImage = 0;
	g_bih.biClrImportant = 0;
	g_bih.biClrUsed = 0;
	g_bih.biXPelsPerMeter = 0;
	g_bih.biYPelsPerMeter = 0;
	g_bih.biPlanes = 1;

	 bool ret = m_CamDS.OpenCamera(0, false, 320, 240, OnVideoCap, this);
	 if (!ret)
	 {
		 AfxMessageBox("Open Cam error");
	 }
	 m_CamDS.Run();
}

void CClientDlg::OnBnClickedButtonClose()
{
	// TODO: 在此添加控件通知处理程序代码
	m_CamDS.Stop();
	m_CamDS.CloseCamera();
	m_enc.Enc_UnInit();
	m_dec.Dec_UnInit();
	m_Sess.Close();
//	fclose(pf);
}

void CClientDlg::OnVideoCap(int nID, BYTE* buff, long len, void* lpParam)
{
	CClientDlg* pThis = (CClientDlg*)lpParam;
	pThis->ProcessVCapData(nID, buff, len);
}


void CClientDlg::ProcessVCapData(int nID, BYTE* buff, long len)
{
	m_pLocal->SetImage((LPBITMAPINFO)&g_bih, buff, FALSE);
	m_enc.encode_frame(buff, len);
}


void CClientDlg::VDecodeCB(BYTE* pData, UINT nLen, void* lpParam)
{
	CClientDlg* pThis = (CClientDlg*)lpParam;
	pThis->ProcessDecodeData(pData, nLen);
}

void CClientDlg::VEncodeCB(BYTE* pData, UINT nLen, void* lpParam)
{
	//if (pf)
	{
	//	fwrite(pData, nLen, 1, pf);
	}
	//return;
	CClientDlg* pThis = (CClientDlg*)lpParam;
	pThis->ProcessEncodeData(pData, nLen);
}

void CClientDlg::ProcessDecodeData(BYTE* pData, UINT nLen)
{
	m_pRemote->SetImage((LPBITMAPINFO)&g_bih, pData, FALSE);
}

void CClientDlg::ProcessEncodeData(BYTE* pData, UINT nLen)
{
	int ret = m_Sess.Send((char*)pData, nLen, inet_addr("127.0.0.1"), 5500);
	TRACE("send ret = %d\n", ret);
}

void CClientDlg::OnUDPDataCB(int sockid, char *data, int len, int ip, int port, void* param)
{
	CClientDlg* pThis = (CClientDlg*)param;
	pThis->OnRecvFrom(sockid, data, len ,ip , port);
}

void CClientDlg::OnRecvFrom(int sockid, char *data, int len, int ip, int port)
{
	if (sockid == m_nSessID)
	{
		TRACE("recv ret = %d\n", len);
		m_dec.decode_frame((BYTE*)data, len);
	}
}