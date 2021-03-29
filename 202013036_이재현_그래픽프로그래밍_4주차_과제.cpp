
#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>
using namespace CK::DD;

// 격자를 그리는 함수
void SoftRenderer::DrawGizmo2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// 그리드 색상
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// 뷰의 영역 계산
	Vector2 viewPos = g.GetMainCamera().GetTransform().GetPosition();
	Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

	// 좌측 하단에서부터 격자 그리기
	int xGridCount = _ScreenSize.X / _Grid2DUnit;
	int yGridCount = _ScreenSize.Y / _Grid2DUnit;

	// 그리드가 시작되는 좌하단 좌표 값 계산
	Vector2 minPos = viewPos - extent;
	Vector2 minGridPos = Vector2(ceilf(minPos.X / (float)_Grid2DUnit), ceilf(minPos.Y / (float)_Grid2DUnit)) * (float)_Grid2DUnit;
	ScreenPoint gridBottomLeft = ScreenPoint::ToScreenCoordinate(_ScreenSize, minGridPos - viewPos);

	for (int ix = 0; ix < xGridCount; ++ix)
	{
		r.DrawFullVerticalLine(gridBottomLeft.X + ix * _Grid2DUnit, gridColor);
	}

	for (int iy = 0; iy < yGridCount; ++iy)
	{
		r.DrawFullHorizontalLine(gridBottomLeft.Y - iy * _Grid2DUnit, gridColor);
	}

	ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
	r.DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
	r.DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}

// 게임 오브젝트 목록

Vector2 currentPosition(0.f, 0.f);

// 최초 씬 로딩을 담당하는 함수
void SoftRenderer::LoadScene2D()
{
	// 최초 씬 로딩에서 사용하는 모듈 내 주요 레퍼런스
	auto& g = Get2DGameEngine();

}
// 게임 로직과 렌더링 로직이 공유하는 변수
float currentScale = 10.f;
float currentDegree = 0.f;
float currentColor = 1.f;
float currentXArea = 1.f;
float currentYArea = 1.f;

// 게임 로직을 담당하는 함수
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	// 게임 로직에서 사용하는 모듈 내 주요 레퍼런스
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	// 게임 로직의 로컬 변수
	static float moveSpeed = 100.f;
	static float scaleSpeed = 20.f;
	static float scaleMin = 5.f;
	static float scaleMax = 10.f;
	static float DegreeMin = 90.f;
	static float DegreeMax = -90.f;
	static float duration = 2.f;
	static float elapsedTime = 0.f;
	static float elapsedTime2 = 0.f;
	static float elapsedTime3 = 0.f;

	//도형의 중심점을 방향키 입력을 받아서 움직임 
	Vector2 inputVector = Vector2(input.GetAxis(InputAxis::XAxis), input.GetAxis(InputAxis::YAxis)).GetNormalize();
	Vector2 deltaPosition = inputVector * moveSpeed * InDeltaSeconds;
	float deltaScale = input.GetAxis(InputAxis::ZAxis) * scaleSpeed * InDeltaSeconds;

	// 경과 시간과 사인함수를 활용한 [0,1]값의 생성
	elapsedTime += InDeltaSeconds;//지난 시간만큼 더해줌
	elapsedTime2 += InDeltaSeconds;
	elapsedTime3 += InDeltaSeconds;
	elapsedTime = Math::FMod(elapsedTime, duration);//1.5초마다 elapsedTime을 0으로 돌림(fmod는 부동소수점의 나머지를 반환하는 함수)
	float currentRad = (elapsedTime / duration) * Math::TwoPI;//TowPI로 한 싸이클 돌았을때 얼마나 돌았는가.
	float alpha = (sinf(currentRad) + 1) * 0.5f;//0과 1 사이의 값
	elapsedTime2 = Math::FMod(elapsedTime2, (duration * 1.9));//루프 타임을 조금씩 어긋나게 만듬
	float currentRad2 = (elapsedTime2 / (duration * 1.9)) * Math::TwoPI;
	float alpha2 = (sinf(currentRad2) + 1) * 0.5f;
	elapsedTime3 = Math::FMod(elapsedTime3, (duration * 0.7));
	float currentRad3 = (elapsedTime3 / (duration * 0.7)) * Math::TwoPI;
	float alpha3 =sinf(currentRad3);
	
	if (scaleMin > 0)
	{
		scaleMin += deltaScale;
		scaleMax += deltaScale;
	}
	else
	{
		scaleMin = 0.1f;
		scaleMax = 5.1f;
	}

	// 물체의 최종 상태 설정
	currentPosition += deltaPosition;
	currentScale = Math::Lerp(scaleMin, scaleMax, alpha) ;//0과 1 사이를 오가는 alpha값으로 스케일을 조절한다.
	currentDegree = Math::Lerp(0.f, 360.f, alpha2);//도형을 회전시킴 
	currentColor = Math::Lerp(1.f, 3.f, alpha2);//HSV에서 색깔을 담당하는 H의 크기를 바꾸어줌 (모든 색의 싸이클이 최소 1싸이클에서 최대 3싸이클을 돔)
	currentXArea = Math::Lerp(0.f, 1.f, alpha3);//도형 크기의 X축을 건드려서 도형이 회전하듯이 만듬
	currentYArea = Math::Lerp(0.f, 1.f, alpha);//도형 크기의 Y축을 건드려서 도형이 회전하듯이 만듬
}

// 렌더링 로직을 담당하는 함수
void SoftRenderer::Render2D()
{
	// 렌더링 로직에서 사용하는 모듈 내 주요 레퍼런스
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// 배경에 격자 그리기
	DrawGizmo2D();

	// 렌더링 로직의 로컬 변수
	float rad = 0.f;
	static float increment = 0.001f;
	static std::vector<Vector2> hearts;
	HSVColor hsv(0.f, 1.f, 0.85f);

	// 하트를 구성하는 점 생성
	if (hearts.empty())
	{
		for (rad = 0.f; rad < Math::TwoPI; rad += increment)
		{
			float sin = sinf(rad);
			float cos = cosf(rad);
			float cos2 = cosf(2 * rad);
			float cos3 = cosf(3 * rad);
			float cos4 = cosf(4 * rad);
			float x = 16.f * sin * sin * sin;
			float y = 13 * cos - 5 * cos2 - 2 * cos3 - cos4;
			hearts.push_back(Vector2(x, y));
		}
	}

	// 각도에 해당하는 사인과 코사인 값 얻기
	float sin = 0.f, cos = 0.f;
	Math::GetSinCos(sin, cos, currentDegree);

	// 각 값을 초기화한 후 색상을 증가시키면서 점에 대응
	rad = 0.f;
	for (auto const& v : hearts)
	{
		// 1. 점에 크기를 적용한다.
		Vector2 scaledV = v * currentScale;
		// 2. 크기가 변한 점을 회전시킨다. (currentXArea와 currentYArea축을 건드려서 도형의 모양을 추가로 바꿈)
		Vector2 rotatedV = Vector2(scaledV.X * cos - scaledV.Y * sin *currentXArea, scaledV.X * sin + scaledV.Y * cos * currentXArea);
		// 3. 회전시킨 점을 이동한다.
		Vector2 translatedV = rotatedV + currentPosition;

		hsv.H = (rad / Math::TwoPI) * currentColor;
		r.DrawPoint(translatedV, hsv.ToLinearColor());
		rad += increment;
	}

	// 현재 위치, 크기, 각도를 화면에 출력
	r.PushStatisticText(std::string("Position : ") + currentPosition.ToString());
	r.PushStatisticText(std::string("Scale : ") + std::to_string(currentScale));
	r.PushStatisticText(std::string("Degree : ") + std::to_string(currentDegree));
}
// 메시를 그리는 함수
void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
}

// 삼각형을 그리는 함수
void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
}
