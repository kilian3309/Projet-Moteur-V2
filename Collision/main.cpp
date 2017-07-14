/*
Choses � savoir:
Vertex : https://en.wikipedia.org/wiki/Vertex_(geometry)

HRESULT : C'est une valeur 32 bit qui est divis�e en 3 parties : un code de s�v�rit�, un code d'installation, un code d'erreur
- Le code de s�v�rit� indique si la valeur repr�sente une info, un warning ou une erreur
- Le code d'installation (facility) indique quelle partie du syst�me est responsable de l'erreur (ne marche que si c'est une erreur)
- Le code d'erreur repr�sente l'erreur, le warning ou l'info
HRESULT peut �tre converti en exception si le code o� il est return ne propose pas de solution afin de traiter les erreurs

LRESULT : Valeur relativement chelou qui donne la taille de la variable et qui poss�de une sorte de base de donn�e afin de l'interpr�ter...
Ce type peut prendre d'autres types (il peut �tre un int, un float...).

XMVECTOR : Type comprenant 4 valeur 32 bit de type float dont l'alignement et le mappage ont �t� optimis� au maximum pour des vecteur (hardware)
Ce type est plus fr�quement utilis� pour des variables local ou global
Quand l'on veut faire un calcul depuis un XFLOAT(1 ou 2 ou 3 ou 4), il vaut mieu passer par un XMVECTOR (on peut convertir l'un vers l'autre facilement)

XMMATRIX : Une matrix de 4*4 align� en 16bit qui enfait est une repr�sentation de 4 XMVECTOR avec une interface afin de les utiliser comme une matrix
Repr�sentation d'une matrix:
_11, _12, _13, _14,
_21, _22, _23, _24,
_31, _32, _33, _34,
_41, _42, _43, _44

XMFLOAT3 : Vecteur 3D : contient 3 coordonn�es en float nomm�es x, y, z rien de plus
Ce type est plus utilis� pour les membres de class

XMVECTORF32 : Comme un XMVECTOR mais portable (entre les OS) et qui contient des syntax pour intialiser correctement les XMVECTOR

FXMVECTOR : Renvoi vers un XMVECTOR
Ce type est plus utilis� pour les arguments de fonction

CXMMATRIX : Renvoi vers XMMATRIX, sert sur certaines architectures � appeler des arguments de fonction dans le bon orde
Ce type est uniquement utilis� pour les arguments de fonction

FXMVECTOR : Renvoi vers XMVECTOR, jour le m�me r�le que CXMMATRIX
Ce type est uniquement utilis� pour les arguments de fonction


*/

#include "DXUT.h"
#include "DXUTgui.h"
#include "DXUTmisc.h"
#include "DXUTCamera.h"
#include "DXUTSettingsDlg.h"
#include "SDKmisc.h"
#include "resource.h"

#include <DirectXColors.h>

#include "CommonStates.h"
#include "Effects.h"
#include "PrimitiveBatch.h"
#include "VertexTypes.h"

#include "hitbox.h"
#include "hud.h"
#include "level.h"

#include "LoadingScreen.h"
#include <string>
#include <thread>
#include <future>

//"The formal parameter is not referenced in the body of the function. The unreferenced parameter is ignored."
#pragma warning( disable : 4100 )

using namespace DirectX;


//A UTILISER QU'EN DEBUG !!!!
#include <codecvt>
template<typename T>std::wstring to_wstring(T what) {
	std::wostringstream ss;
	ss << chat;
	return ss.str();
}

template<>std::wstring to_wstring(wchar_t what) {
	return std::wstring(1, what);
}

template<>std::wstring to_wstring(int what) {
	return std::to_wstring(what);
}

template<>std::wstring to_wstring(float what) {
	return std::to_wstring(what);
}

template<>std::wstring to_wstring(double what) {
	return std::to_wstring(what);
}

template<>std::wstring to_wstring(long long what) {
	return std::to_wstring(what);
}

template<>std::wstring to_wstring(long what) {
	return std::to_wstring(what);
}

template<>std::wstring to_wstring(long double what) {
	return std::to_wstring(what);
}

template<>std::wstring to_wstring(unsigned long long what) {
	return std::to_wstring(what);
}

template<>std::wstring to_wstring(unsigned int what) {
	return std::to_wstring(what);
}

template<>std::wstring to_wstring(unsigned long what) {
	return std::to_wstring(what);
}

template<>std::wstring to_wstring(std::wstring what) {
	return what;
}

template<>std::wstring to_wstring(std::string str) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convertX;
	return convertX.from_bytes(str);
}





//FIN UTILISATION DEBUG

//Constantes


//Variables globales
std::unique_ptr<CommonStates>                           g_States;

bool isMenuOpen;

//Declarations des fonction pour pouvoir les utiliser dans WinMain
//Note: Pour conna�tre leur fonctionnement, arguments, ... -> go � la d�finition
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext);
void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext);
void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext);
void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext);
bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext);
HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext);
void CALLBACK OnD3D11DestroyDevice(void* pUserContext);
void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext);

void InitApp();
void RenderText();

void InitializeObjects();
void Animate(double fTime);
void Collide();
void RenderObjects();
//void SetViewForGroup(int group);

void DrawGrid(FXMVECTOR xAxis, FXMVECTOR yAxis, FXMVECTOR origin, size_t xdivs, size_t ydivs, GXMVECTOR color);
void DrawFrustum(const BoundingFrustum& frustum, FXMVECTOR color);
void DrawAabb(const BoundingBox& box, FXMVECTOR color);
void DrawObb(const BoundingOrientedBox& obb, FXMVECTOR color);
void DrawSphere(const BoundingSphere& sphere, FXMVECTOR color);
void DrawRay(FXMVECTOR Origin, FXMVECTOR Direction, bool bNormalize, FXMVECTOR color);
void DrawTriangle(FXMVECTOR PointA, FXMVECTOR PointB, FXMVECTOR PointC, CXMVECTOR color);

//TestCollisionLevel l;
level1 l;

debugHUD g_debugHUD(&g_DialogResourceManager, &g_SettingsDlg);

infoHUD* g_infoHUD;


void OpenMenu() {
	isMenuOpen = true;


}

int wWinMainEnd() {

	//level0 l1;
	//l1.InitApp();
	//l1.test();


	//Configure les fonctions que doit appeler directX pour ses diff�rentes actions
	//Note: On appelle sa des fonctions de rappel
	DXUTSetCallbackMsgProc(MsgProc);
	DXUTSetCallbackKeyboard(OnKeyboard);
	DXUTSetCallbackFrameMove(OnFrameMove);
	DXUTSetCallbackDeviceChanging(ModifyDeviceSettings);
	DXUTSetCallbackD3D11DeviceAcceptable(IsD3D11DeviceAcceptable);
	DXUTSetCallbackD3D11DeviceCreated(OnD3D11CreateDevice);
	DXUTSetCallbackD3D11SwapChainResized(OnD3D11ResizedSwapChain);
	DXUTSetCallbackD3D11SwapChainReleasing(OnD3D11ReleasingSwapChain);
	DXUTSetCallbackD3D11DeviceDestroyed(OnD3D11DestroyDevice);
	DXUTSetCallbackD3D11FrameRender(OnD3D11FrameRender);
	IncrementLoading();
	InitApp();
	IncrementLoading();
	//Parse les parametres de la command line, affiche les msbox comme des erreurs, pas de param�tres en plus
	DXUTInit(true, true, nullptr);
	IncrementLoading();
	DXUTSetCursorSettings(true, true); //Affiche le curseur et on l'attache au plein �cran
	IncrementLoading();

	DXUTCreateWindow(L"ISN  Motor V2");
	IncrementLoading();


	DXUTCreateDevice(D3D_FEATURE_LEVEL_10_0, true, 800, 600);
	IncrementLoading();

	DXUTMainLoop(); //DXUT loop de render

	ReleaseLoadingScreen();
	return DXUTGetExitCode();
}

/*
WINAPI : Entr�e de l'API windows (fonction main() mais pour windows)
hInstance : Instance principale du programme : genre de conteneur qui repr�sente le programme et tout ce qu'il contien
hPrevInstance : Argument anciennement utilis� (Windows 16bits) donc toujours == NULL
lpCmdLine : Les arguments de la commande si le programme est d�mar� via cmd ( ex: Si le programme s'appelle 'pgrm' et que l'on �x�cute cette ligne: "pgrm --help --version" alors lpCmdLine=L"--help --version"
nCmdShow : Le mode d'affichage de

*/
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	//V�rification de la m�moire lors du run
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//DirectX utilise les instructions SSE2 sur Windows. On v�rifi donc qu'elles sont bien support�es le plus t�t possible
	if (!XMVerifyCPUSupport())
	{
		MessageBox(NULL, TEXT("This application requires the processor support SSE2 instructions."),
			TEXT("Collision"), MB_OK | MB_ICONEXCLAMATION);
		return -1;
	}

	//On cr�� 2 thread ind�pendants : Le premier s'occupe du chargement et le 2� doit finir la WinMain (sa �vite le "ne r�pond pas", enfait non x) -> � faire : le processus de l'interface doit avoir une priorit�
	//sup�rieur � celle du work thread pour r�pondre h24)
	std::async(std::launch::async, CreateLoadingScreen, hInstance, LoadingMax);

	//WinMain Returned Status
	auto WRS = std::async(wWinMainEnd);


	return WRS.get();
}


/*
Initialisation de l'application
*/
inline void InitApp()
{
	l.InitApp();
	g_SettingsDlg.Init(&g_DialogResourceManager);
	InitializeObjects();
}


/*
Faire le render des caracteristiques de tout ce qui touche au graphique (carte, fps ...)
*/
inline void RenderText()
{
	
}


/*
Initialisation des diff�rents objets sur la map
*/
inline void InitializeObjects()
{
	l.InitializeObjets();
}


/*
Fait bouger tous les objets de la sc�ne
fTime : Temps actuel (InGame)
*/
inline void Animate(double fTime)
{
	l.Animate(fTime);
}


/*
Cette fonction met � jour toutes les collisions pour les tester
*/
inline void Collide()
{
	l.Collide();
}

/*
Rendre les objets � collision (c'est pas tr�s francais je sait)
*/
inline void RenderObjects()
{
	l.RenderObjects();
}


/*
Si l'on a besoin de fonctionnalit�(s) avanc�(s), certaines combinaison de r�glage (r�solution, plein �cran, REFs...) peuvent �tre refus�s
Nous n'utilisons pas de fonctionnalit�s avanc�s donc on accepte tout
*/
inline bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext)
{
	return true;
}


/*
Cette fonction est appel�e apr�s que le device soit cr��. En g�n�rale dans cette fonction on:
- Alloue la m�moire
- R�gle les buffers
- Autre

*/
HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	HRESULT hr;

	auto pd3dImmediateContext = DXUTGetD3D11DeviceContext();
	V_RETURN(g_DialogResourceManager.OnD3D11CreateDevice(pd3dDevice, pd3dImmediateContext));
	V_RETURN(g_SettingsDlg.OnD3D11CreateDevice(pd3dDevice));


	g_infoHUD = new infoHUD(pd3dDevice, pd3dImmediateContext, &g_DialogResourceManager);
	g_infoHUD->Init();

	//Creation d'autres resources de render
	g_States.reset(new CommonStates(pd3dDevice));
	OnDrawingCreateDevice(pd3dDevice, pd3dImmediateContext);
	l.OnCreateDevice();
	//Setup du debugHUD
		
	if (g_debugHUD.isInit)
		g_debugHUD.m_hud.GetButton(GUI_TOGGLEWARP)->SetEnabled(true);
	
	return S_OK;
}


/*
Creation des resources D3D11 qui n�c�ssitent le back buffer
*/
HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	HRESULT hr;

	V_RETURN(g_DialogResourceManager.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));
	V_RETURN(g_SettingsDlg.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));
	
	l.OnResizedSwapChain(pBackBufferSurfaceDesc);
	g_debugHUD.OnResizedSwapChain(pBackBufferSurfaceDesc);
	

	return S_OK;
}


/*
Render en utilisant le D3D Device, cette fonction est appel�e � chaque trame (si elle est red�ssin�e uniquement).
Ici on applique les effets, les resources et on d�ssine
*/
void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
	float fElapsedTime, void* pUserContext)
{
	if (g_SettingsDlg.IsActive())
	{
		g_SettingsDlg.OnRender(fElapsedTime);
		return;
	}

	auto pRTV = DXUTGetD3D11RenderTargetView();
	pd3dImmediateContext->ClearRenderTargetView(pRTV, Colors::MidnightBlue);

	// Clear le depth stencil
	auto pDSV = DXUTGetD3D11DepthStencilView();
	pd3dImmediateContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0, 0);

	//R�cup�ration de la projection et de la matrix de view depuis la class camera
	
	XMMATRIX mWorld = l.GetCamera()->GetWorldMatrix();
	XMMATRIX mView = l.GetCamera()->GetViewMatrix();
	XMMATRIX mProj = l.GetCamera()->GetProjMatrix();

	_drg_BatchEffect->SetWorld(mWorld);
	_drg_BatchEffect->SetView(mView);
	_drg_BatchEffect->SetProjection(mProj); 

	RenderObjects();

	//Render le HUD (faudra lui trouver un nom de spy)
	DXUT_BeginPerfEvent(DXUT_PERFEVENTCOLOR, L"HUD / Stats");

	g_debugHUD.OnFrameRender(&fElapsedTime);
	//g_groupHUD.OnFrameRender(&fElapsedTime);
	l.OnFrameRender(fElapsedTime);

	//RenderText();
	XMVECTOR LookAtCamera(l.GetCamera()->GetLookAtPt());

	g_infoHUD->addAdditionalInfos((std::to_wstring(XMVectorGetIntX(LookAtCamera))+strToWstr(" ")+std::to_wstring(XMVectorGetIntY(LookAtCamera))).c_str());
	g_infoHUD->Render();

	DXUT_EndPerfEvent();

	static ULONGLONG timefirst = GetTickCount64();
	if (GetTickCount64() - timefirst > 5000)
	{
		OutputDebugString(DXUTGetFrameStats(DXUTIsVsyncEnabled()));
		OutputDebugString(L"\n");
		timefirst = GetTickCount64();
	}
}



/*
Lib�ration de la m�moire allou�e par la swap chain dans OnD3D11ResizedSwapChain
*/
inline void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext)
{
	g_DialogResourceManager.OnD3D11ReleasingSwapChain();

}


/*
Lib�ration de la m�moire des resources cr��es dans OnD3D11CreateDevice
*/
void CALLBACK OnD3D11DestroyDevice(void* pUserContext)
{
	g_DialogResourceManager.OnD3D11DestroyDevice();
	g_SettingsDlg.OnD3D11DestroyDevice();
	DXUTGetGlobalResourceCache().OnDestroyDevice();

	g_States.reset();

	OnDrawingDestroyDevice();

	SAFE_DELETE(g_infoHUD);

}


/*
Modifications ant�rieurs � la cr�ation du device par DXUT (utile pour des fonctionnalit�s avanc� mais vu qu'on en a pas...)
*/
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
	//Implementation MSAAx8
	static bool s_bFirstTime = true;
	if (s_bFirstTime) {
		DXGI_SAMPLE_DESC MSAA4xSampleDesc = { 8, 0 };
		pDeviceSettings->d3d11.sd.SampleDesc = MSAA4xSampleDesc;
	}
	return true;
}


/*
Cette fonction met � jour la sc�ne. Son nom peut changer en fonction de l'API D3D utilis�e
fTime : Temps actuel (InGame)
fEnlapsedTime : Temps depuis la derni�re update
*/
inline void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
	//Mise � jour des positions des objets
	Animate(fTime);

	//Mise � jour des collisions
	Collide();

	//Mise � jour de la camera
	//g_Camera.FrameMove(fElapsedTime);
	l.OnFrameMove(fElapsedTime);
	
}


/*
Cette fonction est la fonction o� toutes les procedures li�es � la fen�tre sont g�r�es
hWnd : Fen�tre principale
uMsg : Message envoy� par la boucle �v�nementielle (boucle principale du programme)
wParam & lParam : Pr�cisions sur l'origine du messageg

LRESULT : La valeur retourn�e d�pend (son type aussi) du message � process
CALLBACK : Pr�cise que la fonction doit �tre appel�e de la mani�re la plus standard possible (__stdcall ici -> de droite � gauche et de haut en bas)
*/
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
	void* pUserContext)
{
	//Passage des messages vers le dialog resource manager
	*pbNoFurtherProcessing = g_DialogResourceManager.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;

	//Passage des messages au dialogue de param�tre
	if (g_SettingsDlg.IsActive())
	{
		g_SettingsDlg.MsgProc(hWnd, uMsg, wParam, lParam);
		return 0;
	}

	//Donne une change au dialogue de prendre le message en 1er
	if (g_debugHUD.isInit) {
		*pbNoFurtherProcessing = g_debugHUD.m_hud.MsgProc(hWnd, uMsg, wParam, lParam);
		if (*pbNoFurtherProcessing)
			return 0;
	}
	if (typeid(l) == typeid(TestCollisionLevel)) {
		*pbNoFurtherProcessing = l.GetGroupHUD()->m_hud.MsgProc(hWnd, uMsg, wParam, lParam);
		if (*pbNoFurtherProcessing)
			return 0;
	}
	


	//g_Camera.HandleMessages(hWnd, uMsg, wParam, lParam);
	l.GetCamera()->HandleMessages(hWnd, uMsg, wParam, lParam);

	return 0;
}



/*
Cette fonction est la pour capter ce qui sort du clavier donc surtout les touches efonc�es ou non
nChar : Num�ro de la touche
bKeyDown : si la touche est appuy�
bAltDown : si la touche ALT est appuy� en m�me temps que la touche
*/
void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext)
{
	switch (nChar)
	{
	case VK_ESCAPE:
		OpenMenu();
		break;
	case VK_F10: 
	{
		g_debugHUD.Init(false);
		g_debugHUD.swapVisibility();
	}
		break;
	default:
		l.OnKeyboard(nChar, bKeyDown, bAltDown, pUserContext);
		break;
	}
}