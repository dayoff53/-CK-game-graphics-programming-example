
#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>
using namespace CK::DD;

// ���ڸ� �׸��� �Լ�
void SoftRenderer::DrawGizmo2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// �׸��� ����
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// ���� ���� ���
	Vector2 viewPos = g.GetMainCamera().GetTransform().GetPosition();
	Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

	// ���� �ϴܿ������� ���� �׸���
	int xGridCount = _ScreenSize.X / _Grid2DUnit;
	int yGridCount = _ScreenSize.Y / _Grid2DUnit;

	// �׸��尡 ���۵Ǵ� ���ϴ� ��ǥ �� ���
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

// ���� ������Ʈ ���

Vector2 currentPosition(0.f, 0.f);

// ���� �� �ε��� ����ϴ� �Լ�
void SoftRenderer::LoadScene2D()
{
	// ���� �� �ε����� ����ϴ� ��� �� �ֿ� ���۷���
	auto& g = Get2DGameEngine();

}
// ���� ������ ������ ������ �����ϴ� ����
float currentScale = 10.f;
float currentDegree = 0.f;
float currentColor = 1.f;
float currentXArea = 1.f;
float currentYArea = 1.f;

// ���� ������ ����ϴ� �Լ�
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	// ���� �������� ����ϴ� ��� �� �ֿ� ���۷���
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	// ���� ������ ���� ����
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

	//������ �߽����� ����Ű �Է��� �޾Ƽ� ������ 
	Vector2 inputVector = Vector2(input.GetAxis(InputAxis::XAxis), input.GetAxis(InputAxis::YAxis)).GetNormalize();
	Vector2 deltaPosition = inputVector * moveSpeed * InDeltaSeconds;
	float deltaScale = input.GetAxis(InputAxis::ZAxis) * scaleSpeed * InDeltaSeconds;

	// ��� �ð��� �����Լ��� Ȱ���� [0,1]���� ����
	elapsedTime += InDeltaSeconds;//���� �ð���ŭ ������
	elapsedTime2 += InDeltaSeconds;
	elapsedTime3 += InDeltaSeconds;
	elapsedTime = Math::FMod(elapsedTime, duration);//1.5�ʸ��� elapsedTime�� 0���� ����(fmod�� �ε��Ҽ����� �������� ��ȯ�ϴ� �Լ�)
	float currentRad = (elapsedTime / duration) * Math::TwoPI;//TowPI�� �� ����Ŭ �������� �󸶳� ���Ҵ°�.
	float alpha = (sinf(currentRad) + 1) * 0.5f;//0�� 1 ������ ��
	elapsedTime2 = Math::FMod(elapsedTime2, (duration * 1.9));//���� Ÿ���� ���ݾ� ��߳��� ����
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

	// ��ü�� ���� ���� ����
	currentPosition += deltaPosition;
	currentScale = Math::Lerp(scaleMin, scaleMax, alpha) ;//0�� 1 ���̸� ������ alpha������ �������� �����Ѵ�.
	currentDegree = Math::Lerp(0.f, 360.f, alpha2);//������ ȸ����Ŵ 
	currentColor = Math::Lerp(1.f, 3.f, alpha2);//HSV���� ������ ����ϴ� H�� ũ�⸦ �ٲپ��� (��� ���� ����Ŭ�� �ּ� 1����Ŭ���� �ִ� 3����Ŭ�� ��)
	currentXArea = Math::Lerp(0.f, 1.f, alpha3);//���� ũ���� X���� �ǵ���� ������ ȸ���ϵ��� ����
	currentYArea = Math::Lerp(0.f, 1.f, alpha);//���� ũ���� Y���� �ǵ���� ������ ȸ���ϵ��� ����
}

// ������ ������ ����ϴ� �Լ�
void SoftRenderer::Render2D()
{
	// ������ �������� ����ϴ� ��� �� �ֿ� ���۷���
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// ��濡 ���� �׸���
	DrawGizmo2D();

	// ������ ������ ���� ����
	float rad = 0.f;
	static float increment = 0.001f;
	static std::vector<Vector2> hearts;
	HSVColor hsv(0.f, 1.f, 0.85f);

	// ��Ʈ�� �����ϴ� �� ����
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

	// ������ �ش��ϴ� ���ΰ� �ڻ��� �� ���
	float sin = 0.f, cos = 0.f;
	Math::GetSinCos(sin, cos, currentDegree);

	// �� ���� �ʱ�ȭ�� �� ������ ������Ű�鼭 ���� ����
	rad = 0.f;
	for (auto const& v : hearts)
	{
		// 1. ���� ũ�⸦ �����Ѵ�.
		Vector2 scaledV = v * currentScale;
		// 2. ũ�Ⱑ ���� ���� ȸ����Ų��. (currentXArea�� currentYArea���� �ǵ���� ������ ����� �߰��� �ٲ�)
		Vector2 rotatedV = Vector2(scaledV.X * cos - scaledV.Y * sin *currentXArea, scaledV.X * sin + scaledV.Y * cos * currentXArea);
		// 3. ȸ����Ų ���� �̵��Ѵ�.
		Vector2 translatedV = rotatedV + currentPosition;

		hsv.H = (rad / Math::TwoPI) * currentColor;
		r.DrawPoint(translatedV, hsv.ToLinearColor());
		rad += increment;
	}

	// ���� ��ġ, ũ��, ������ ȭ�鿡 ���
	r.PushStatisticText(std::string("Position : ") + currentPosition.ToString());
	r.PushStatisticText(std::string("Scale : ") + std::to_string(currentScale));
	r.PushStatisticText(std::string("Degree : ") + std::to_string(currentDegree));
}
// �޽ø� �׸��� �Լ�
void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
}

// �ﰢ���� �׸��� �Լ�
void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
}
