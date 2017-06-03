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
#include <DirectXCollision.h>

#include "CommonStates.h"
#include "Effects.h"
#include "PrimitiveBatch.h"
#include "VertexTypes.h"

//"The formal parameter is not referenced in the body of the function. The unreferenced parameter is ignored."
#pragma warning( disable : 4100 )

using namespace DirectX;

//Objets � hitbox
struct CollisionSphere
{
	BoundingSphere sphere;
	ContainmentType collision;
};

struct CollisionBox
{
	BoundingOrientedBox obox;
	ContainmentType collision;
};

struct CollisionAABox
{
	BoundingBox aabox;
	ContainmentType collision;
};

struct CollisionFrustum
{
	BoundingFrustum frustum;
	ContainmentType collision;
};

struct CollisionTriangle
{
	XMVECTOR pointa;
	XMVECTOR pointb;
	XMVECTOR pointc;
	ContainmentType collision;
};

struct CollisionRay
{
	XMVECTOR origin;
	XMVECTOR direction;
};

//Constantes
const int GROUP_COUNT = 4;			//Nombre de groupes o� il y a des objets
const int CAMERA_COUNT = 4;			//Nombre d'endroits o� peut aller la cam�ra
const float CAMERA_SPACING = 50.f;	

//Variables globales
CModelViewerCamera          g_Camera;					//La camera
/*
	Ici dialog signifie que c'est lui qui g�re les resources partarg�s
*/
CDXUTDialogResourceManager  g_DialogResourceManager;	//Manager pour les resources partarg�s des diff�rents dialogs
CD3DSettingsDlg             g_SettingsDlg;				//Parametre du device principale
CDXUTTextHelper*            g_pTxtHelper = nullptr;		//Afficheur de text
CDXUTDialog                 g_HUD;						//Dialog pour les controles standards
CDXUTDialog                 g_SampleUI;					//Dialogue pour les controles sp�cifiques (controles au sens par exemple des touches cheloux)

ID3D11InputLayout*                  g_pBatchInputLayout = nullptr;

std::unique_ptr<CommonStates>                           g_States;
std::unique_ptr<BasicEffect>                            g_BatchEffect;
std::unique_ptr<PrimitiveBatch<VertexPositionColor>>    g_Batch;

//Objets primaires (les gros quoi)
BoundingFrustum g_PrimaryFrustum;			//Frustrum : Cone (� base circulaire ou car�e) sans pointe du coup le sommet est plat. Vu de cot� : Trap�ze
BoundingOrientedBox g_PrimaryOrientedBox;
BoundingBox g_PrimaryAABox;
CollisionRay g_PrimaryRay;

//Objets secondaires (les petits)
CollisionSphere     g_SecondarySpheres[GROUP_COUNT];
CollisionBox        g_SecondaryOrientedBoxes[GROUP_COUNT];
CollisionAABox      g_SecondaryAABoxes[GROUP_COUNT];
CollisionTriangle   g_SecondaryTriangles[GROUP_COUNT];

//Box � l'endroit o� le ray coupe un objet
CollisionAABox g_RayHitResultBox;

//Liste des emplacements de la cam�ra
XMVECTOR g_CameraOrigins[CAMERA_COUNT];

//Variables de la fen�tre de chargement
HWND hwnPB;

//Contr�les des bouttons � droite (UI control)
#define IDC_STATIC              -1
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3
#define IDC_TOGGLEWARP          4
#define IDC_GROUP               5


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
void SetViewForGroup(int group);

void DrawGrid(FXMVECTOR xAxis, FXMVECTOR yAxis, FXMVECTOR origin, size_t xdivs, size_t ydivs, GXMVECTOR color);
void DrawFrustum(const BoundingFrustum& frustum, FXMVECTOR color);
void DrawAabb(const BoundingBox& box, FXMVECTOR color);
void DrawObb(const BoundingOrientedBox& obb, FXMVECTOR color);
void DrawSphere(const BoundingSphere& sphere, FXMVECTOR color);
void DrawRay(FXMVECTOR Origin, FXMVECTOR Direction, bool bNormalize, FXMVECTOR color);
void DrawTriangle(FXMVECTOR PointA, FXMVECTOR PointB, FXMVECTOR PointC, CXMVECTOR color);

LRESULT CALLBACK MsgProcLS(HWND win, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE:
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(win, msg, wParam, lParam);
	}
}

void CreateLoadingScreen(HINSTANCE hInst = (HINSTANCE)nullptr, int maxRange=10) {
	if (!hInst) {
		hInst = (HINSTANCE)GetModuleHandle(nullptr);
	}

	WNDCLASS WCLoadScreen;
	WCLoadScreen.style = 0;
	WCLoadScreen.lpfnWndProc = MsgProcLS;
	WCLoadScreen.cbClsExtra = NULL;
	WCLoadScreen.cbWndExtra = NULL;
	WCLoadScreen.hInstance = hInst;
	WCLoadScreen.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WCLoadScreen.hCursor = LoadCursor(NULL, IDC_ARROW);
	WCLoadScreen.hbrBackground = (HBRUSH)(1 + COLOR_BTNFACE);
	WCLoadScreen.lpszMenuName = NULL;
	WCLoadScreen.lpszClassName = L"WCClass";

	RegisterClass(&WCLoadScreen);

	//hwnPB = CreateWindow(L"WCClass", L"Loading Engine...", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 200, 100, NULL, NULL, hInst, NULL);
	hwnPB = CreateWindowEx(0, PROGRESS_CLASS, L"Loading Engine...", WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 200, 100, NULL, (HMENU)0, hInst, NULL);
	SendMessage(hwnPB, PBM_SETRANGE, 0, MAKELPARAM(0, maxRange));
	SendMessage(hwnPB, PBM_SETSTEP, (WPARAM)1, 0);

}

void IncrementLoading() {
	SendMessage(hwnPB, PBM_STEPIT, 0, 0);
}

void DestroyLoading() {
	DestroyWindow(hwnPB);
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

	//DirectX utilise les instructions SSE2 sur Windows. On v�rifi donc qu'elle sont bien support�es le plus t�t possible
	if (!XMVerifyCPUSupport())
	{
		MessageBox(NULL, TEXT("This application requires the processor support SSE2 instructions."),
			TEXT("Collision"), MB_OK | MB_ICONEXCLAMATION);
		return -1;
	}
	CreateLoadingScreen();
	MessageBox(NULL, TEXT("h"), TEXT("hj"), MB_OK);
	IncrementLoading();
	MessageBox(NULL, TEXT("h"), TEXT("hj"), MB_OK);
	DestroyLoading();



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

	InitApp();
	//Parse les parametres de la command line, affiche les msbox comme des erreurs, pas de param�tres en plus
	DXUTInit(true, true, nullptr);
	DXUTSetCursorSettings(true, true); //Affiche le curseur et on l'attache au plein �cran
	DXUTCreateWindow(L"ISN  Motor V2");


	DXUTCreateDevice(D3D_FEATURE_LEVEL_10_0, true, 800, 600);

	DXUTMainLoop(); //DXUT loop de render

	return DXUTGetExitCode();
}


/*
	Initialisation de l'application
*/
void InitApp()
{
	g_SettingsDlg.Init(&g_DialogResourceManager);
	g_HUD.Init(&g_DialogResourceManager);
	g_SampleUI.Init(&g_DialogResourceManager);

	g_HUD.SetCallback(OnGUIEvent);
	int iY = 30;
	int iYo = 26;
	g_HUD.AddButton(IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 0, iY, 170, 22);
	g_HUD.AddButton(IDC_CHANGEDEVICE, L"Change device (F2)", 0, iY += iYo, 170, 22, VK_F2);
	g_HUD.AddButton(IDC_TOGGLEREF, L"Toggle REF (F3)", 0, iY += iYo, 170, 22, VK_F3);
	g_HUD.AddButton(IDC_TOGGLEWARP, L"Toggle WARP (F4)", 0, iY += iYo, 170, 22, VK_F4);

	g_SampleUI.SetCallback(OnGUIEvent);

	CDXUTComboBox* pComboBox = nullptr;
	g_SampleUI.AddStatic(IDC_STATIC, L"(G)roup", 10, 0, 170, 25);
	g_SampleUI.AddComboBox(IDC_GROUP, 0, 25, 170, 24, 'G', false, &pComboBox);
	if (pComboBox)
		pComboBox->SetDropHeight(50);

	pComboBox->AddItem(L"Frustum", IntToPtr(0));
	pComboBox->AddItem(L"Axis-aligned Box", IntToPtr(1));
	pComboBox->AddItem(L"Oriented Box", IntToPtr(2));
	pComboBox->AddItem(L"Ray", IntToPtr(3));

	InitializeObjects();
}


/*
Faire le render des caracteristiques de tout ce qui touche au graphique (carte, fps ...)
*/
void RenderText()
{
	g_pTxtHelper->Begin();
	g_pTxtHelper->SetInsertionPos(5, 5);
	g_pTxtHelper->SetForegroundColor(Colors::Yellow);
	g_pTxtHelper->DrawTextLine(DXUTGetFrameStats(DXUTIsVsyncEnabled()));
	g_pTxtHelper->DrawTextLine(DXUTGetDeviceStats());
	g_pTxtHelper->End();
}


/*
	Initialisation des diff�rents objets sur la map
*/
void InitializeObjects()
{
	const XMVECTOR XMZero = XMVectorZero();

	//construction du primary frustum depuis une matrix de projection de D3D
	//Note: Cela peut �tre aussi fait depuis la matriux de projection de la camera
	XMMATRIX xmProj = XMMatrixPerspectiveFovLH(XM_PIDIV4, 1.77778f, 0.5f, 10.0f);
	BoundingFrustum::CreateFromMatrix(g_PrimaryFrustum, xmProj);
	g_PrimaryFrustum.Origin.z = -7.0f;
	g_CameraOrigins[0] = XMVectorSet(0, 0, 0, 0);

	//Construction de l'AABox
	g_PrimaryAABox.Center = XMFLOAT3(CAMERA_SPACING, 0, 0);
	g_PrimaryAABox.Extents = XMFLOAT3(5, 5, 5);
	g_CameraOrigins[1] = XMVectorSet(CAMERA_SPACING, 0, 0, 0);

	//Construction de l'OBox
	g_PrimaryOrientedBox.Center = XMFLOAT3(-CAMERA_SPACING, 0, 0);
	g_PrimaryOrientedBox.Extents = XMFLOAT3(5, 5, 5);
	XMStoreFloat4(&g_PrimaryOrientedBox.Orientation, XMQuaternionRotationRollPitchYaw(XM_PIDIV4, XM_PIDIV4, 0));
	g_CameraOrigins[2] = XMVectorSet(-CAMERA_SPACING, 0, 0, 0);

	//Construction du ray
	g_PrimaryRay.origin = XMVectorSet(0, 0, CAMERA_SPACING, 0);
	g_PrimaryRay.direction = g_XMIdentityR2;
	g_CameraOrigins[3] = XMVectorSet(0, 0, CAMERA_SPACING, 0);

	//Initialisation de tous les objets secondaires avec les valeurs par d�faut
	for (UINT i = 0; i < GROUP_COUNT; i++)
	{
		g_SecondarySpheres[i].sphere.Radius = 1.0f;
		g_SecondarySpheres[i].sphere.Center = XMFLOAT3(0, 0, 0);
		g_SecondarySpheres[i].collision = DISJOINT;

		g_SecondaryOrientedBoxes[i].obox.Center = XMFLOAT3(0, 0, 0);
		g_SecondaryOrientedBoxes[i].obox.Extents = XMFLOAT3(0.5f, 0.5f, 0.5f);
		g_SecondaryOrientedBoxes[i].obox.Orientation = XMFLOAT4(0, 0, 0, 1);
		g_SecondaryOrientedBoxes[i].collision = DISJOINT;

		g_SecondaryAABoxes[i].aabox.Center = XMFLOAT3(0, 0, 0);
		g_SecondaryAABoxes[i].aabox.Extents = XMFLOAT3(0.5f, 0.5f, 0.5f);
		g_SecondaryAABoxes[i].collision = DISJOINT;

		g_SecondaryTriangles[i].pointa = XMZero;
		g_SecondaryTriangles[i].pointb = XMZero;
		g_SecondaryTriangles[i].pointc = XMZero;
		g_SecondaryTriangles[i].collision = DISJOINT;
	}

	//Construction de l'AABox qui est le r�sultat de la collision avec le ray
	g_RayHitResultBox.aabox.Center = XMFLOAT3(0, 0, 0);
	g_RayHitResultBox.aabox.Extents = XMFLOAT3(0.05f, 0.05f, 0.05f);
}


/*
	Fait bouger tous les objets de la sc�ne
	fTime : Temps actuel (InGame)
*/
void Animate(double fTime)
{
	float t = (FLOAT)(fTime * 0.2);

	const float camera0OriginX = XMVectorGetX(g_CameraOrigins[0]);
	const float camera1OriginX = XMVectorGetX(g_CameraOrigins[1]);
	const float camera2OriginX = XMVectorGetX(g_CameraOrigins[2]);
	const float camera3OriginX = XMVectorGetX(g_CameraOrigins[3]);
	const float camera3OriginZ = XMVectorGetZ(g_CameraOrigins[3]);

	//Animation de la sphere 0 autour du frustum
	g_SecondarySpheres[0].sphere.Center.x = 10 * sinf(3 * t);
	g_SecondarySpheres[0].sphere.Center.y = 7 * cosf(5 * t);

	//Animation de la oriented box 0 autour du frustum
	g_SecondaryOrientedBoxes[0].obox.Center.x = 8 * sinf(3.5f * t);
	g_SecondaryOrientedBoxes[0].obox.Center.y = 5 * cosf(5.1f * t);
	XMStoreFloat4(&(g_SecondaryOrientedBoxes[0].obox.Orientation), XMQuaternionRotationRollPitchYaw(t * 1.4f,
		t * 0.2f,
		t));

	//Animation de la box (Axis-Aligned) autour de l'oriented box
	g_SecondaryAABoxes[0].aabox.Center.x = 10 * sinf(2.1f * t);
	g_SecondaryAABoxes[0].aabox.Center.y = 7 * cosf(3.8f * t);

	//Animation de la sphere 1 autour de l'AABox 0
	g_SecondarySpheres[1].sphere.Center.x = 8 * sinf(2.9f * t) + camera1OriginX;
	g_SecondarySpheres[1].sphere.Center.y = 8 * cosf(4.6f * t);
	g_SecondarySpheres[1].sphere.Center.z = 8 * cosf(1.6f * t);

	//Animation de" l'Obox autour de l'AABox
	g_SecondaryOrientedBoxes[1].obox.Center.x = 8 * sinf(3.2f * t) + camera1OriginX;
	g_SecondaryOrientedBoxes[1].obox.Center.y = 8 * cosf(2.1f * t);
	g_SecondaryOrientedBoxes[1].obox.Center.z = 8 * sinf(1.6f * t);
	XMStoreFloat4(&(g_SecondaryOrientedBoxes[1].obox.Orientation), XMQuaternionRotationRollPitchYaw(t * 0.7f,
		t * 1.3f,
		t));

	//Animation de l'Obox autour de l'Obox
	g_SecondaryAABoxes[1].aabox.Center.x = 8 * sinf(1.1f * t) + camera1OriginX;
	g_SecondaryAABoxes[1].aabox.Center.y = 8 * cosf(5.8f * t);
	g_SecondaryAABoxes[1].aabox.Center.z = 8 * cosf(3.0f * t);

	//Animation de la sphere 2 autour de l'Obox
	g_SecondarySpheres[2].sphere.Center.x = 8 * sinf(2.2f * t) + camera2OriginX;
	g_SecondarySpheres[2].sphere.Center.y = 8 * cosf(4.3f * t);
	g_SecondarySpheres[2].sphere.Center.z = 8 * cosf(1.8f * t);


	g_SecondaryOrientedBoxes[2].obox.Center.x = 8 * sinf(3.7f * t) + camera2OriginX;
	g_SecondaryOrientedBoxes[2].obox.Center.y = 8 * cosf(2.5f * t);
	g_SecondaryOrientedBoxes[2].obox.Center.z = 8 * sinf(1.1f * t);
	XMStoreFloat4(&(g_SecondaryOrientedBoxes[2].obox.Orientation), XMQuaternionRotationRollPitchYaw(t * 0.9f,
		t * 1.8f,
		t));

	g_SecondaryAABoxes[2].aabox.Center.x = 8 * sinf(1.3f * t) + camera2OriginX;
	g_SecondaryAABoxes[2].aabox.Center.y = 8 * cosf(5.2f * t);
	g_SecondaryAABoxes[2].aabox.Center.z = 8 * cosf(3.5f * t);

	//Triangles �quilateraux avec un rayon de 2
	const XMVECTOR TrianglePointA = { 0, 2, 0, 0 };
	const XMVECTOR TrianglePointB = { 1.732f, -1, 0, 0 };
	const XMVECTOR TrianglePointC = { -1.732f, -1, 0, 0 };

	//Animation du triangle 0 autour du frustum
	XMMATRIX TriangleCoords = XMMatrixRotationRollPitchYaw(t * 1.4f, t * 2.5f, t);
	XMMATRIX Translation = XMMatrixTranslation(5 * sinf(5.3f * t) + camera0OriginX,
		5 * cosf(2.3f * t),
		5 * sinf(3.4f * t));
	TriangleCoords = XMMatrixMultiply(TriangleCoords, Translation);
	g_SecondaryTriangles[0].pointa = XMVector3Transform(TrianglePointA, TriangleCoords);
	g_SecondaryTriangles[0].pointb = XMVector3Transform(TrianglePointB, TriangleCoords);
	g_SecondaryTriangles[0].pointc = XMVector3Transform(TrianglePointC, TriangleCoords);

	//Animation du triangle 1 autour de l'oriented box
	TriangleCoords = XMMatrixRotationRollPitchYaw(t * 1.4f, t * 2.5f, t);
	Translation = XMMatrixTranslation(8 * sinf(5.3f * t) + camera1OriginX,
		8 * cosf(2.3f * t),
		8 * sinf(3.4f * t));
	TriangleCoords = XMMatrixMultiply(TriangleCoords, Translation);
	g_SecondaryTriangles[1].pointa = XMVector3Transform(TrianglePointA, TriangleCoords);
	g_SecondaryTriangles[1].pointb = XMVector3Transform(TrianglePointB, TriangleCoords);
	g_SecondaryTriangles[1].pointc = XMVector3Transform(TrianglePointC, TriangleCoords);

	// Animation du triangle 2 autour de l'oriented box
	TriangleCoords = XMMatrixRotationRollPitchYaw(t * 1.4f, t * 2.5f, t);
	Translation = XMMatrixTranslation(8 * sinf(5.3f * t) + camera2OriginX,
		8 * cosf(2.3f * t),
		8 * sinf(3.4f * t));
	TriangleCoords = XMMatrixMultiply(TriangleCoords, Translation);
	g_SecondaryTriangles[2].pointa = XMVector3Transform(TrianglePointA, TriangleCoords);
	g_SecondaryTriangles[2].pointb = XMVector3Transform(TrianglePointB, TriangleCoords);
	g_SecondaryTriangles[2].pointc = XMVector3Transform(TrianglePointC, TriangleCoords);

	//Animation du ray (Seule objet primaire anim�)
	g_PrimaryRay.direction = XMVectorSet(sinf(t * 3), 0, cosf(t * 3), 0);

	//Animation de la sphere 3 autour du ray
	g_SecondarySpheres[3].sphere.Center = XMFLOAT3(camera3OriginX - 3,
		0.5f * sinf(t * 5),
		camera3OriginZ);

	//Animation de la box 3 autour du ray
	g_SecondaryAABoxes[3].aabox.Center = XMFLOAT3(camera3OriginX + 3,
		0.5f * sinf(t * 4),
		camera3OriginZ);

	//Animation de l'oriented box 3 autour du ray
	g_SecondaryOrientedBoxes[3].obox.Center = XMFLOAT3(camera3OriginX,
		0.5f * sinf(t * 4.5f),
		camera3OriginZ + 3);
	XMStoreFloat4(&(g_SecondaryOrientedBoxes[3].obox.Orientation), XMQuaternionRotationRollPitchYaw(t * 0.9f,
		t * 1.8f,
		t));

	//Animation du triangle 3 autour du ray
	TriangleCoords = XMMatrixRotationRollPitchYaw(t * 1.4f, t * 2.5f, t);
	Translation = XMMatrixTranslation(camera3OriginX,
		0.5f * cosf(4.3f * t),
		camera3OriginZ - 3);
	TriangleCoords = XMMatrixMultiply(TriangleCoords, Translation);
	g_SecondaryTriangles[3].pointa = XMVector3Transform(TrianglePointA, TriangleCoords);
	g_SecondaryTriangles[3].pointb = XMVector3Transform(TrianglePointB, TriangleCoords);
	g_SecondaryTriangles[3].pointc = XMVector3Transform(TrianglePointC, TriangleCoords);
}


/*
	Cette fonction met � jour toutes les collisions pour les tester
*/
void Collide()
{
	//Test les collisions entre les objets et le frustum
	g_SecondarySpheres[0].collision = g_PrimaryFrustum.Contains(g_SecondarySpheres[0].sphere);
	g_SecondaryOrientedBoxes[0].collision = g_PrimaryFrustum.Contains(g_SecondaryOrientedBoxes[0].obox);
	g_SecondaryAABoxes[0].collision = g_PrimaryFrustum.Contains(g_SecondaryAABoxes[0].aabox);
	g_SecondaryTriangles[0].collision = g_PrimaryFrustum.Contains(g_SecondaryTriangles[0].pointa,
		g_SecondaryTriangles[0].pointb,
		g_SecondaryTriangles[0].pointc);

	//Test les collisions entre les objets et l'anligned box
	g_SecondarySpheres[1].collision = g_PrimaryAABox.Contains(g_SecondarySpheres[1].sphere);
	g_SecondaryOrientedBoxes[1].collision = g_PrimaryAABox.Contains(g_SecondaryOrientedBoxes[1].obox);
	g_SecondaryAABoxes[1].collision = g_PrimaryAABox.Contains(g_SecondaryAABoxes[1].aabox);
	g_SecondaryTriangles[1].collision = g_PrimaryAABox.Contains(g_SecondaryTriangles[1].pointa,
		g_SecondaryTriangles[1].pointb,
		g_SecondaryTriangles[1].pointc);

	//Test les collisions entre le objets et la box
	g_SecondarySpheres[2].collision = g_PrimaryOrientedBox.Contains(g_SecondarySpheres[2].sphere);
	g_SecondaryOrientedBoxes[2].collision = g_PrimaryOrientedBox.Contains(g_SecondaryOrientedBoxes[2].obox);
	g_SecondaryAABoxes[2].collision = g_PrimaryOrientedBox.Contains(g_SecondaryAABoxes[2].aabox);
	g_SecondaryTriangles[2].collision = g_PrimaryOrientedBox.Contains(g_SecondaryTriangles[2].pointa,
		g_SecondaryTriangles[2].pointb,
		g_SecondaryTriangles[2].pointc);

	//Test les collisions entre les objets et la ray
	float fDistance = -1.0f;

	float fDist;
	if (g_SecondarySpheres[3].sphere.Intersects(g_PrimaryRay.origin, g_PrimaryRay.direction, fDist))
	{
		fDistance = fDist;
		g_SecondarySpheres[3].collision = INTERSECTS;
	}
	else
		g_SecondarySpheres[3].collision = DISJOINT;

	if (g_SecondaryOrientedBoxes[3].obox.Intersects(g_PrimaryRay.origin, g_PrimaryRay.direction, fDist))
	{
		fDistance = fDist;
		g_SecondaryOrientedBoxes[3].collision = INTERSECTS;
	}
	else
		g_SecondaryOrientedBoxes[3].collision = DISJOINT;

	if (g_SecondaryAABoxes[3].aabox.Intersects(g_PrimaryRay.origin, g_PrimaryRay.direction, fDist))
	{
		fDistance = fDist;
		g_SecondaryAABoxes[3].collision = INTERSECTS;
	}
	else
		g_SecondaryAABoxes[3].collision = DISJOINT;

	if (TriangleTests::Intersects(g_PrimaryRay.origin, g_PrimaryRay.direction,
		g_SecondaryTriangles[3].pointa,
		g_SecondaryTriangles[3].pointb,
		g_SecondaryTriangles[3].pointc,
		fDist))
	{
		fDistance = fDist;
		g_SecondaryTriangles[3].collision = INTERSECTS;
	}
	else
		g_SecondaryTriangles[3].collision = DISJOINT;

	//Si un des test des intersections du ray est bon, fDistance sera positive
	if (fDistance > 0)
	{
		//Le primary ray est suppos� normalis�
		XMVECTOR HitLocation = XMVectorMultiplyAdd(g_PrimaryRay.direction, XMVectorReplicate(fDistance),
			g_PrimaryRay.origin);
		XMStoreFloat3(&g_RayHitResultBox.aabox.Center, HitLocation);
		g_RayHitResultBox.collision = INTERSECTS;
	}
	else
	{
		g_RayHitResultBox.collision = DISJOINT;
	}
}


/*
	Retourne la couleur de l'objet en fonction de la collision
*/
inline XMVECTOR GetCollisionColor(ContainmentType collision, int groupnumber)
{
	if (groupnumber >= 3 && collision > 0)
		collision = CONTAINS;

	switch (collision)
	{
	case DISJOINT:      return Colors::Green;
	case INTERSECTS:    return Colors::Yellow;
	case CONTAINS:
	default:            return Colors::Red;
	}
}


/*
	Rendre les objets � collision (c'est pas tr�s francais je sait)
*/
void RenderObjects()
{
	//Draw les planes du sol (les grilles quoi)
	for (int i = 0; i < CAMERA_COUNT; ++i)
	{
		static const XMVECTORF32 s_vXAxis = { 20.f, 0.f, 0.f, 0.f };
		static const XMVECTORF32 s_vYAxis = { 0.f, 0.f, 20.f, 0.f };

		static const XMVECTORF32 s_Offset = { 0.f, 10.f, 0.f, 0.f };
		XMVECTOR vOrigin = g_CameraOrigins[i] - s_Offset;

		const int iXDivisions = 20;
		const int iYDivisions = 20;
		DrawGrid(s_vXAxis, s_vYAxis, vOrigin, iXDivisions, iYDivisions, Colors::Black);
	}

	//Draw les objets de collisions primary en blanc
	DrawFrustum(g_PrimaryFrustum, Colors::White);
	DrawAabb(g_PrimaryAABox, Colors::White);
	DrawObb(g_PrimaryOrientedBox, Colors::White);

	{
		XMVECTOR Direction = XMVectorScale(g_PrimaryRay.direction, 10.0f);
		DrawRay(g_PrimaryRay.origin, Direction, false, Colors::LightGray);
		DrawRay(g_PrimaryRay.origin, Direction, false, Colors::White);
	}

	//Draw les objets de collision secondaires (ceux qui bougent) en couleur en se basant sur les resultats des collisions
	for (int i = 0; i < GROUP_COUNT; ++i)
	{
		const CollisionSphere& sphere = g_SecondarySpheres[i];
		XMVECTOR c = GetCollisionColor(sphere.collision, i);
		DrawSphere(sphere.sphere, c);

		const CollisionBox& obox = g_SecondaryOrientedBoxes[i];
		c = GetCollisionColor(obox.collision, i);
		DrawObb(obox.obox, c);

		const CollisionAABox& aabox = g_SecondaryAABoxes[i];
		c = GetCollisionColor(aabox.collision, i);
		DrawAabb(aabox.aabox, c);

		const CollisionTriangle& tri = g_SecondaryTriangles[i];
		c = GetCollisionColor(tri.collision, i);
		DrawTriangle(tri.pointa, tri.pointb, tri.pointc, c);
	}

	//Draw le resultat de la collision du ray (le petit carr�)
	if (g_RayHitResultBox.collision != DISJOINT)
		DrawAabb(g_RayHitResultBox.aabox, Colors::Yellow);
}


/*
Cette fonction met la camera sur une vue particuli�re au groupe d'objet
*/

void SetViewForGroup(int group)
{
	assert(group < GROUP_COUNT);

	g_Camera.Reset();

	static const XMVECTORF32 s_Offset0 = { 0.f, 20.f, 20.f, 0.f };
	static const XMVECTORF32 s_Offset = { 0.f, 20.f, -20.f, 0.f };
	XMVECTOR vecEye = g_CameraOrigins[group] + ((group == 0) ? s_Offset0 : s_Offset);

	g_Camera.SetViewParams(vecEye, g_CameraOrigins[group]);

	XMFLOAT3 vecAt;
	XMStoreFloat3(&vecAt, g_CameraOrigins[group]);
	g_Camera.SetModelCenter(vecAt);
}


/*
	Dessiner une grille
*/
void DrawGrid(FXMVECTOR xAxis, FXMVECTOR yAxis, FXMVECTOR origin, size_t xdivs, size_t ydivs, GXMVECTOR color)
{
	auto context = DXUTGetD3D11DeviceContext();
	g_BatchEffect->Apply(context);

	context->IASetInputLayout(g_pBatchInputLayout);

	g_Batch->Begin();

	xdivs = std::max<size_t>(1, xdivs);
	ydivs = std::max<size_t>(1, ydivs);

	for (size_t i = 0; i <= xdivs; ++i)
	{
		float fPercent = float(i) / float(xdivs);
		fPercent = (fPercent * 2.0f) - 1.0f;
		XMVECTOR vScale = XMVectorScale(xAxis, fPercent);
		vScale = XMVectorAdd(vScale, origin);

		//D�cris une couleur sur un Vertex
		VertexPositionColor v1(XMVectorSubtract(vScale, yAxis), color);
		VertexPositionColor v2(XMVectorAdd(vScale, yAxis), color);
		g_Batch->DrawLine(v1, v2);
	}

	for (size_t i = 0; i <= ydivs; i++)
	{
		FLOAT fPercent = float(i) / float(ydivs);
		fPercent = (fPercent * 2.0f) - 1.0f;
		XMVECTOR vScale = XMVectorScale(yAxis, fPercent);
		vScale = XMVectorAdd(vScale, origin);

		VertexPositionColor v1(XMVectorSubtract(vScale, xAxis), color);
		VertexPositionColor v2(XMVectorAdd(vScale, xAxis), color);
		g_Batch->DrawLine(v1, v2);
	}

	g_Batch->End();
}


/*
	Dessiner un Frustum
*/
void DrawFrustum(const BoundingFrustum& frustum, FXMVECTOR color)
{
	XMFLOAT3 corners[BoundingFrustum::CORNER_COUNT];
	frustum.GetCorners(corners);

	VertexPositionColor verts[24];
	verts[0].position = corners[0];
	verts[1].position = corners[1];
	verts[2].position = corners[1];
	verts[3].position = corners[2];
	verts[4].position = corners[2];
	verts[5].position = corners[3];
	verts[6].position = corners[3];
	verts[7].position = corners[0];

	verts[8].position = corners[0];
	verts[9].position = corners[4];
	verts[10].position = corners[1];
	verts[11].position = corners[5];
	verts[12].position = corners[2];
	verts[13].position = corners[6];
	verts[14].position = corners[3];
	verts[15].position = corners[7];

	verts[16].position = corners[4];
	verts[17].position = corners[5];
	verts[18].position = corners[5];
	verts[19].position = corners[6];
	verts[20].position = corners[6];
	verts[21].position = corners[7];
	verts[22].position = corners[7];
	verts[23].position = corners[4];

	for (size_t j = 0; j < _countof(verts); ++j)
	{
		XMStoreFloat4(&verts[j].color, color);
	}

	auto context = DXUTGetD3D11DeviceContext();
	g_BatchEffect->Apply(context);

	context->IASetInputLayout(g_pBatchInputLayout);

	g_Batch->Begin();

	g_Batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, verts, _countof(verts));

	g_Batch->End();
}

/*
	Dessiner un cube depuis une matrix
*/
void DrawCube(CXMMATRIX mWorld, FXMVECTOR color)
{
	static const XMVECTOR s_verts[8] =
	{
		{ -1, -1, -1, 0 },
		{ 1, -1, -1, 0 },
		{ 1, -1, 1, 0 },
		{ -1, -1, 1, 0 },
		{ -1, 1, -1, 0 },
		{ 1, 1, -1, 0 },
		{ 1, 1, 1, 0 },
		{ -1, 1, 1, 0 }
	};
	static const WORD s_indices[] =
	{
		0, 1,
		1, 2,
		2, 3,
		3, 0,
		4, 5,
		5, 6,
		6, 7,
		7, 4,
		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	VertexPositionColor verts[8];
	for (int i = 0; i < 8; ++i)
	{
		XMVECTOR v = XMVector3Transform(s_verts[i], mWorld);
		XMStoreFloat3(&verts[i].position, v);
		XMStoreFloat4(&verts[i].color, color);
	}

	auto context = DXUTGetD3D11DeviceContext();
	g_BatchEffect->Apply(context);

	context->IASetInputLayout(g_pBatchInputLayout);

	g_Batch->Begin();

	g_Batch->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, s_indices, _countof(s_indices), verts, 8);

	g_Batch->End();
}


/*
	Dessiner un AABox
*/
void DrawAabb(const BoundingBox& box, FXMVECTOR color)
{
	XMMATRIX matWorld = XMMatrixScaling(box.Extents.x, box.Extents.y, box.Extents.z);
	XMVECTOR position = XMLoadFloat3(&box.Center);
	matWorld.r[3] = XMVectorSelect(matWorld.r[3], position, g_XMSelect1110);

	DrawCube(matWorld, color);
}


/*
	Dessiner un OBox
*/
void DrawObb(const BoundingOrientedBox& obb, FXMVECTOR color)
{
	XMMATRIX matWorld = XMMatrixRotationQuaternion(XMLoadFloat4(&obb.Orientation));
	XMMATRIX matScale = XMMatrixScaling(obb.Extents.x, obb.Extents.y, obb.Extents.z);
	matWorld = XMMatrixMultiply(matScale, matWorld);
	XMVECTOR position = XMLoadFloat3(&obb.Center);
	matWorld.r[3] = XMVectorSelect(matWorld.r[3], position, g_XMSelect1110);

	DrawCube(matWorld, color);
}


/*
	Dessiner un anneau
*/
void DrawRing(FXMVECTOR Origin, FXMVECTOR MajorAxis, FXMVECTOR MinorAxis, CXMVECTOR color)
{
	static const DWORD dwRingSegments = 32;

	VertexPositionColor verts[dwRingSegments + 1];

	FLOAT fAngleDelta = XM_2PI / (float)dwRingSegments;
	//Au lieu d'utiliser cos/sin pour chaque segment, on calcul
	//le signe de l'angle delta et on calcul sin et cos depuis sa
	XMVECTOR cosDelta = XMVectorReplicate(cosf(fAngleDelta));
	XMVECTOR sinDelta = XMVectorReplicate(sinf(fAngleDelta));
	XMVECTOR incrementalSin = XMVectorZero();
	static const XMVECTOR initialCos =
	{
		1.0f, 1.0f, 1.0f, 1.0f
	};
	XMVECTOR incrementalCos = initialCos;
	for (DWORD i = 0; i < dwRingSegments; i++)
	{
		XMVECTOR Pos;
		Pos = XMVectorMultiplyAdd(MajorAxis, incrementalCos, Origin);
		Pos = XMVectorMultiplyAdd(MinorAxis, incrementalSin, Pos);
		XMStoreFloat3(&verts[i].position, Pos);
		XMStoreFloat4(&verts[i].color, color);
		//Formule pour rotate un vecteur
		XMVECTOR newCos = incrementalCos * cosDelta - incrementalSin * sinDelta;
		XMVECTOR newSin = incrementalCos * sinDelta + incrementalSin * cosDelta;
		incrementalCos = newCos;
		incrementalSin = newSin;
	}
	verts[dwRingSegments] = verts[0];

	auto context = DXUTGetD3D11DeviceContext();
	g_BatchEffect->Apply(context);

	context->IASetInputLayout(g_pBatchInputLayout);

	g_Batch->Begin();

	g_Batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, dwRingSegments + 1);

	g_Batch->End();
}


/*
	Dessiner une sphere
*/
void DrawSphere(const BoundingSphere& sphere, FXMVECTOR color)
{
	XMVECTOR origin = XMLoadFloat3(&sphere.Center);

	const float fRadius = sphere.Radius;

	XMVECTOR xaxis = g_XMIdentityR0 * fRadius;
	XMVECTOR yaxis = g_XMIdentityR1 * fRadius;
	XMVECTOR zaxis = g_XMIdentityR2 * fRadius;

	DrawRing(origin, xaxis, zaxis, color);
	DrawRing(origin, xaxis, yaxis, color);
	DrawRing(origin, yaxis, zaxis, color);
}


/*
	Dessiner un rayon
*/
void DrawRay(FXMVECTOR Origin, FXMVECTOR Direction, bool bNormalize, FXMVECTOR color)
{
	VertexPositionColor verts[3];
	XMStoreFloat3(&verts[0].position, Origin);

	XMVECTOR NormDirection = XMVector3Normalize(Direction);
	XMVECTOR RayDirection = (bNormalize) ? NormDirection : Direction;

	XMVECTOR PerpVector = XMVector3Cross(NormDirection, g_XMIdentityR1);

	if (XMVector3Equal(XMVector3LengthSq(PerpVector), g_XMZero))
	{
		PerpVector = XMVector3Cross(NormDirection, g_XMIdentityR2);
	}
	PerpVector = XMVector3Normalize(PerpVector);

	XMStoreFloat3(&verts[1].position, XMVectorAdd(RayDirection, Origin));
	PerpVector = XMVectorScale(PerpVector, 0.0625f);
	NormDirection = XMVectorScale(NormDirection, -0.25f);
	RayDirection = XMVectorAdd(PerpVector, RayDirection);
	RayDirection = XMVectorAdd(NormDirection, RayDirection);
	XMStoreFloat3(&verts[2].position, XMVectorAdd(RayDirection, Origin));

	XMStoreFloat4(&verts[0].color, color);
	XMStoreFloat4(&verts[1].color, color);
	XMStoreFloat4(&verts[2].color, color);

	auto context = DXUTGetD3D11DeviceContext();
	g_BatchEffect->Apply(context);

	context->IASetInputLayout(g_pBatchInputLayout);

	g_Batch->Begin();

	g_Batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, 2);

	g_Batch->End();
}


/*
	Dessiner un triangle
*/
void DrawTriangle(FXMVECTOR PointA, FXMVECTOR PointB, FXMVECTOR PointC, CXMVECTOR color)
{
	VertexPositionColor verts[4];
	XMStoreFloat3(&verts[0].position, PointA);
	XMStoreFloat3(&verts[1].position, PointB);
	XMStoreFloat3(&verts[2].position, PointC);
	XMStoreFloat3(&verts[3].position, PointA);

	XMStoreFloat4(&verts[0].color, color);
	XMStoreFloat4(&verts[1].color, color);
	XMStoreFloat4(&verts[2].color, color);
	XMStoreFloat4(&verts[3].color, color);

	auto context = DXUTGetD3D11DeviceContext();
	g_BatchEffect->Apply(context);

	context->IASetInputLayout(g_pBatchInputLayout);

	g_Batch->Begin();

	g_Batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, 4);

	g_Batch->End();
}


/*
	Si l'on a besoin de fonctionnalit�(s) avanc�(s), certaines combinaison de r�glage (r�solution, plein �cran, REFs...) peuvent �tre refus�s
	Nous n'utilisons pas de fonctionnalit�s avanc�s donc on accepte tout
*/
bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext)
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
	g_pTxtHelper = new CDXUTTextHelper(pd3dDevice, pd3dImmediateContext, &g_DialogResourceManager, 15);

	//Creation d'autres resources de render
	g_States.reset(new CommonStates(pd3dDevice));
	g_Batch.reset(new PrimitiveBatch<VertexPositionColor>(pd3dImmediateContext));

	g_BatchEffect.reset(new BasicEffect(pd3dDevice));
	g_BatchEffect->SetVertexColorEnabled(true);

	{
		void const* shaderByteCode;
		size_t byteCodeLength;

		g_BatchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

		hr = pd3dDevice->CreateInputLayout(VertexPositionColor::InputElements,
			VertexPositionColor::InputElementCount,
			shaderByteCode, byteCodeLength,
			&g_pBatchInputLayout);
		if (FAILED(hr))
			return hr;
	}

	//Setup des param�tres de la camera
	auto pComboBox = g_SampleUI.GetComboBox(IDC_GROUP);
	SetViewForGroup((pComboBox) ? (int)PtrToInt(pComboBox->GetSelectedData()) : 0);

	g_HUD.GetButton(IDC_TOGGLEWARP)->SetEnabled(true);

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

	//Setup des param�tres de projection de la cam�ra
	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
	g_Camera.SetProjParams(XM_PI / 4, fAspectRatio, 0.1f, 1000.0f);
	g_Camera.SetWindow(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
	g_Camera.SetButtonMasks(MOUSE_LEFT_BUTTON, MOUSE_WHEEL, MOUSE_MIDDLE_BUTTON);

	g_HUD.SetLocation(pBackBufferSurfaceDesc->Width - 170, 0);
	g_HUD.SetSize(170, 170);
	g_SampleUI.SetLocation(pBackBufferSurfaceDesc->Width - 170, pBackBufferSurfaceDesc->Height - 300);
	g_SampleUI.SetSize(170, 300);

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
	XMMATRIX mWorld = g_Camera.GetWorldMatrix();
	XMMATRIX mView = g_Camera.GetViewMatrix();
	XMMATRIX mProj = g_Camera.GetProjMatrix();

	g_BatchEffect->SetWorld(mWorld);
	g_BatchEffect->SetView(mView);
	g_BatchEffect->SetProjection(mProj);

	RenderObjects();

	//Render le HUD (faudra lui trouver un nom de spy)
	DXUT_BeginPerfEvent(DXUT_PERFEVENTCOLOR, L"HUD / Stats");
	g_HUD.OnRender(fElapsedTime);
	g_SampleUI.OnRender(fElapsedTime);
	RenderText();
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
void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext)
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
	SAFE_DELETE(g_pTxtHelper);

	g_States.reset();
	g_BatchEffect.reset();
	g_Batch.reset();

	SAFE_RELEASE(g_pBatchInputLayout);
}


/*
	Modifications ant�rieurs � la cr�ation du device par DXUT (utile pour des fonctionnalit�s avanc� mais vu qu'on en a pas...)
*/
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
	return true;
}


/*
	Cette fonction met � jour la sc�ne. Son nom peut changer en fonction de l'API D3D utilis�e
	fTime : Temps actuel (InGame)
	fEnlapsedTime : Temps depuis la derni�re update
*/
void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
	//Mise � jour des positions des objets
	Animate(fTime);

	//Mise � jour des collisions
	Collide();

	//Mise � jour de la camera
	g_Camera.FrameMove(fElapsedTime);
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
	*pbNoFurtherProcessing = g_HUD.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;
	*pbNoFurtherProcessing = g_SampleUI.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;


	g_Camera.HandleMessages(hWnd, uMsg, wParam, lParam);

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
	case '1':
	case '2':
	case '3':
	case '4':
	{
		int group = (nChar - '1');
		auto pComboBox = g_SampleUI.GetComboBox(IDC_GROUP);
		assert(pComboBox != NULL);
		pComboBox->SetSelectedByData(IntToPtr(group));
		SetViewForGroup(group);
	}
	break;
	}
}


/*
	Cette fonction est appel�e si on clique sur un des bouttons du GUI
	nEvent : Num�ro de l'event (Dans notre cas, on ne traite que les cliques donc on ignore ce param�tre)
	nControlID : Num�ro du boutton
	pControl : Pointeur pour controller le device
*/
void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext)
{
	switch (nControlID)
	{
	case IDC_TOGGLEFULLSCREEN:
		DXUTToggleFullScreen();
		break;
	case IDC_TOGGLEREF:
		DXUTToggleREF();
		break;
	case IDC_TOGGLEWARP:
		DXUTToggleWARP();
		break;
	case IDC_CHANGEDEVICE:
		g_SettingsDlg.SetActive(!g_SettingsDlg.IsActive());
		break;
	case IDC_GROUP:
	{
		auto pComboBox = reinterpret_cast<CDXUTComboBox*>(pControl);
		SetViewForGroup((int)PtrToInt(pComboBox->GetSelectedData()));
	}
	break;
	}
}
