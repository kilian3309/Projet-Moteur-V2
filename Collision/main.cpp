/*
Choses à savoir:
Vertex : https://en.wikipedia.org/wiki/Vertex_(geometry)

HRESULT : C'est une valeur 32 bit qui est divisée en 3 parties : un code de sévérité, un code d'installation, un code d'erreur
- Le code de sévérité indique si la valeur représente une info, un warning ou une erreur
- Le code d'installation (facility) indique quelle partie du système est responsable de l'erreur (ne marche que si c'est une erreur)
- Le code d'erreur représente l'erreur, le warning ou l'info
HRESULT peut être converti en exception si le code où il est return ne propose pas de solution afin de traiter les erreurs

LRESULT : Valeur relativement chelou qui donne la taille de la variable et qui possède une sorte de base de donnée afin de l'interpréter...
Ce type peut prendre d'autres types (il peut être un int, un float...).

XMVECTOR : Type comprenant 4 valeur 32 bit de type float dont l'alignement et le mappage ont été optimisé au maximum pour des vecteur (hardware)
Ce type est plus fréquement utilisé pour des variables local ou global
Quand l'on veut faire un calcul depuis un XFLOAT(1 ou 2 ou 3 ou 4), il vaut mieu passer par un XMVECTOR (on peut convertir l'un vers l'autre facilement)

XMMATRIX : Une matrix de 4*4 aligné en 16bit qui enfait est une représentation de 4 XMVECTOR avec une interface afin de les utiliser comme une matrix
Représentation d'une matrix:
_11, _12, _13, _14,
_21, _22, _23, _24,
_31, _32, _33, _34,
_41, _42, _43, _44

XMFLOAT3 : Vecteur 3D : contient 3 coordonnées en float nommées x, y, z rien de plus
Ce type est plus utilisé pour les membres de class

XMVECTORF32 : Comme un XMVECTOR mais portable (entre les OS) et qui contient des syntax pour intialiser correctement les XMVECTOR

FXMVECTOR : Renvoi vers un XMVECTOR
Ce type est plus utilisé pour les arguments de fonction

CXMMATRIX : Renvoi vers XMMATRIX, sert sur certaines architectures à appeler des arguments de fonction dans le bon orde
Ce type est uniquement utilisé pour les arguments de fonction

FXMVECTOR : Renvoi vers XMVECTOR, jour le même rôle que CXMMATRIX
Ce type est uniquement utilisé pour les arguments de fonction


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
//Note: Pour connaître leur fonctionnement, arguments, ... -> go à la définition
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


	//Configure les fonctions que doit appeler directX pour ses différentes actions
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
	//Parse les parametres de la command line, affiche les msbox comme des erreurs, pas de paramètres en plus
	DXUTInit(true, true, nullptr);
	IncrementLoading();
	DXUTSetCursorSettings(true, true); //Affiche le curseur et on l'attache au plein écran
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
WINAPI : Entrée de l'API windows (fonction main() mais pour windows)
hInstance : Instance principale du programme : genre de conteneur qui représente le programme et tout ce qu'il contien
hPrevInstance : Argument anciennement utilisé (Windows 16bits) donc toujours == NULL
lpCmdLine : Les arguments de la commande si le programme est démaré via cmd ( ex: Si le programme s'appelle 'pgrm' et que l'on éxécute cette ligne: "pgrm --help --version" alors lpCmdLine=L"--help --version"
nCmdShow : Le mode d'affichage de

*/
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	//Vérification de la mémoire lors du run
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//DirectX utilise les instructions SSE2 sur Windows. On vérifi donc qu'elles sont bien supportées le plus tôt possible
	if (!XMVerifyCPUSupport())
	{
		MessageBox(NULL, TEXT("This application requires the processor support SSE2 instructions."),
			TEXT("Collision"), MB_OK | MB_ICONEXCLAMATION);
		return -1;
	}

	//On créé 2 thread indépendants : Le premier s'occupe du chargement et le 2è doit finir la WinMain (sa évite le "ne répond pas", enfait non x) -> à faire : le processus de l'interface doit avoir une priorité
	//supérieur à celle du work thread pour répondre h24)
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
Initialisation des différents objets sur la map
*/
inline void InitializeObjects()
{
	l.InitializeObjets();
}


/*
Fait bouger tous les objets de la scène
fTime : Temps actuel (InGame)
*/
inline void Animate(double fTime)
{
	l.Animate(fTime);
}


/*
Cette fonction met à jour toutes les collisions pour les tester
*/
inline void Collide()
{
	l.Collide();
}

/*
Rendre les objets à collision (c'est pas très francais je sait)
*/
inline void RenderObjects()
{
	l.RenderObjects();
}


/*
Si l'on a besoin de fonctionnalité(s) avancé(s), certaines combinaison de réglage (résolution, plein écran, REFs...) peuvent être refusés
Nous n'utilisons pas de fonctionnalités avancés donc on accepte tout
*/
inline bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext)
{
	return true;
}


/*
Cette fonction est appelée après que le device soit créé. En générale dans cette fonction on:
- Alloue la mémoire
- Règle les buffers
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
Creation des resources D3D11 qui nécéssitent le back buffer
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
Render en utilisant le D3D Device, cette fonction est appelée à chaque trame (si elle est redéssinée uniquement).
Ici on applique les effets, les resources et on déssine
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

	//Récupération de la projection et de la matrix de view depuis la class camera
	
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
Libération de la mémoire allouée par la swap chain dans OnD3D11ResizedSwapChain
*/
inline void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext)
{
	g_DialogResourceManager.OnD3D11ReleasingSwapChain();

}


/*
Libération de la mémoire des resources créées dans OnD3D11CreateDevice
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
Modifications antérieurs à la création du device par DXUT (utile pour des fonctionnalités avancé mais vu qu'on en a pas...)
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
Cette fonction met à jour la scène. Son nom peut changer en fonction de l'API D3D utilisée
fTime : Temps actuel (InGame)
fEnlapsedTime : Temps depuis la dernière update
*/
inline void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
	//Mise à jour des positions des objets
	Animate(fTime);

	//Mise à jour des collisions
	Collide();

	//Mise à jour de la camera
	//g_Camera.FrameMove(fElapsedTime);
	l.OnFrameMove(fElapsedTime);
	
}


/*
Cette fonction est la fonction où toutes les procedures liées à la fenêtre sont gérées
hWnd : Fenêtre principale
uMsg : Message envoyé par la boucle événementielle (boucle principale du programme)
wParam & lParam : Précisions sur l'origine du messageg

LRESULT : La valeur retournée dépend (son type aussi) du message à process
CALLBACK : Précise que la fonction doit être appelée de la manière la plus standard possible (__stdcall ici -> de droite à gauche et de haut en bas)
*/
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
	void* pUserContext)
{
	//Passage des messages vers le dialog resource manager
	*pbNoFurtherProcessing = g_DialogResourceManager.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;

	//Passage des messages au dialogue de paramètre
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
Cette fonction est la pour capter ce qui sort du clavier donc surtout les touches efoncées ou non
nChar : Numéro de la touche
bKeyDown : si la touche est appuyé
bAltDown : si la touche ALT est appuyé en même temps que la touche
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