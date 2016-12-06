#include "template.h"
#include "Scene.h"
#include "Renderer.h"
Renderer* renderer;
// -----------------------------------------------------------
// Initialize the game
// -----------------------------------------------------------
void Game::Init()
{
	Scene* myScene = new Scene();
	renderer = new Renderer(myScene, renderSurface);
}

// -----------------------------------------------------------
// Input handling
// -----------------------------------------------------------
#define VK_W 0x57
#define VK_S 0x53
#define VK_A 0x41
#define VK_D 0x44
#define VK_C 0x43

#define VK_MASK_PRESSED_DOWN 1 << 16
#define DEBUG 0

void Game::HandleInput(float dt)
{
	bool w = GetAsyncKeyState(VK_W) && VK_MASK_PRESSED_DOWN;
	bool a = GetAsyncKeyState(VK_A) && VK_MASK_PRESSED_DOWN;
	bool s = GetAsyncKeyState(VK_S) && VK_MASK_PRESSED_DOWN;
	bool d = GetAsyncKeyState(VK_D) && VK_MASK_PRESSED_DOWN;
	bool c = GetAsyncKeyState(VK_C) && VK_MASK_PRESSED_DOWN;
	bool up = GetAsyncKeyState(VK_UP) && VK_MASK_PRESSED_DOWN;
	bool down = GetAsyncKeyState(VK_DOWN) && VK_MASK_PRESSED_DOWN;
	bool left = GetAsyncKeyState(VK_LEFT) && VK_MASK_PRESSED_DOWN;
	bool right = GetAsyncKeyState(VK_RIGHT) && VK_MASK_PRESSED_DOWN;
	bool rctrl = GetAsyncKeyState(VK_RCONTROL) && VK_MASK_PRESSED_DOWN;
	bool lctrl = GetAsyncKeyState(VK_LCONTROL) && VK_MASK_PRESSED_DOWN;
	bool space = GetAsyncKeyState(VK_SPACE) && VK_MASK_PRESSED_DOWN;
	//
	//#if DEBUG
	//	printf("Handling input! \n");
	//	printf(
	//		"Keystates! \n W	: %i \n A : %i \n S : %i \n D : %i \n C : %i \n UP : %i \n DOWN : %i \n LEFT : %i \n RIGHT : %i \n RCONTROL : %i \n LCONTROL : %i \n SPACEBAR : %i \n",
	//		GetAsyncKeyState(VK_W),
	//		GetAsyncKeyState(VK_A),
	//		GetAsyncKeyState(VK_S),
	//		GetAsyncKeyState(VK_D),
	//		GetAsyncKeyState(VK_C),
	//		GetAsyncKeyState(VK_UP),
	//		GetAsyncKeyState(VK_DOWN),
	//		GetAsyncKeyState(VK_LEFT),
	//		GetAsyncKeyState(VK_RIGHT),
	//		GetAsyncKeyState(VK_RCONTROL),
	//		GetAsyncKeyState(VK_LCONTROL),
	//		GetAsyncKeyState(VK_SPACE));
	//
	//	printf(
	//		"Bool States! \n W	: %i \n A : %i \n S : %i \n D : %i \n C : %i \n UP : %i \n DOWN : %i \n LEFT : %i \n RIGHT : %i \n RCONTROL : %i \n LCONTROL : %i \n SPACEBAR : %i \n",
	//		w,
	//		a,
	//		s,
	//		d,
	//		c,
	//		space,
	//		rctrl,
	//		lctrl,
	//		up,
	//		down,
	//		left,
	//		right
	//		);
	//#endif
	//	Camera* camera = renderer->scene->camera;
	//	mat4 transform = camera->transformMatrix;
	//
	//	float rotationSpeed = 0.005f;
	//	float movementSpeed = 0.001f;
	//
	//	float rotationAmount = rotationSpeed * dt;
	//
	//	vec3 rotationMask = vec3(
	//		(-left + right),
	//		(up + -down),
	//		(rctrl + -lctrl)
	//		);
	//
	//
	//	vec3 screenCenterRotateRight = normalize(camera->screenCenter + camera->rRight * rotationAmount * rotationMask.x);
	//	camera->TransformCamera(inverse(lookAt(camera->position, screenCenterRotateRight - camera->position, camera->wUp)));
	//
	//	vec3 screenCenterRotateUp = normalize(camera->screenCenter + camera->rUp * rotationAmount * rotationMask.y);
	//	camera->TransformCamera(inverse(lookAt(camera->position, screenCenterRotateUp - camera->position, camera->wUp)));
	//
	//
	//
	//	//TODO : add rotate around z?
	//	//vec3 screenCenterRotateForward = normalize(camera->screenCenter + camera->viewDirection * rotationAmount * rotationMask.z);
	//	//transform = inverse(lookAt(camera->position, screenCenterRotateForward - camera->position, camera->wUp));
	//	//camera->TransformCamera(transform);
	//
	//
	//	// Adjust translation based on new viewdirection
	//	vec3 viewDirection = camera->viewDirection;
	//	vec3 rightDirection = camera->rRight;
	//	vec3 currentPosition = camera->position;
	//
	//	vec3 movementMask = vec3(
	//		(a + -d),
	//		(c + -space),
	//		(w + -s)
	//		);
	//
	//	//printf("Movementmask :  %f, %f, %f \n", movementMask.x, movementMask.y, movementMask.z);
	//
	//	float movementAmount = movementSpeed * dt;
	//	movementMask = movementAmount * movementMask;
	//
	//	vec4 col0 = transform[0];
	//	vec4 col1 = transform[1];
	//	vec4 col2 = transform[2];
	//	vec4 col3 = transform[3];
	//	float xW = currentPosition.x + dot(vec4(movementMask.x, movementMask.y, movementMask.z, 1), vec4(col0.x, col1.x, col2.x, 1));	//vec4(transform[0], transform[4], transform[8], 1));
	//	float yW = currentPosition.y + dot(vec4(movementMask.x, movementMask.y, movementMask.z, 1), vec4(col0.y, col1.y, col2.y, 1));	//vec4(transform[1], transform[5], transform[9], 1));
	//	float zW = currentPosition.z + dot(vec4(movementMask.x, movementMask.y, movementMask.z, 1), vec4(col0.z, col1.z, col2.z, 1));	//vec4(transform[2], transform[6], transform[10], 1));
	//
	//	transform = lookAt(vec3(xW, yW, zW), camera->screenCenter + vec3(xW, yW, zW), camera->wUp);
	//	//camera->TransformCamera(transform);
}

void Game::MouseUp(int _Button) { /* implement if you want to detect mouse button presses */ }
void Game::MouseDown(int _Button) { /* implement if you want to detect mouse button presses */ }
void Game::MouseMove(int _X, int _Y) { /* implement if you want to detect mouse movement */ }



void Game::KeyUp(int a_Key) {}
//	Camera* camera = renderer->scene->camera;
//	mat4 transform = camera->transformMatrix;
//
//	float rotationSpeed = 0.005f;
//
//	// Adjust viewdirection
//	if (a_Key == 79 || a_Key == 80)
//	{
//		if (a_Key == 79)
//			transform = rotate(transform,  rotationSpeed, camera->rUp);
//		else
//			transform = rotate(transform, -1*rotationSpeed, camera->rUp);
//		camera->TransformCamera(transform);
//	}
//
//
//	if (a_Key == 82 || a_Key == 81)
//	{
//		if (a_Key == 82)
//			transform = rotate(transform, -1 * rotationSpeed, camera->rRight);
//		else
//			transform = rotate(transform, rotationSpeed, camera->rRight);
//		camera->TransformCamera(transform);
//	}
//}

void Game::KeyDown(int a_Key)
{
	Camera* camera = renderer->scene->camera;
	mat4 transform = camera->transformMatrix;

	float rotationSpeed = 0.05f;

	// Adjust viewdirection
	if (a_Key == 79 || a_Key == 80)
	{
		mat4 previousTransform = transform;
		if (a_Key == 79)
			transform = rotate(transform, -1 * rotationSpeed, camera->rUp);
		else
			transform = rotate(transform, rotationSpeed, camera->rUp);
		camera->TransformCamera(transform);
		camera->transformMatrix = previousTransform;

	}

	if (a_Key == 82 || a_Key == 81)
	{
		mat4 previousTransform = transform;
		if (a_Key == 82)
			transform = rotate(transform, -1 * rotationSpeed, camera->rRight);
		else
			transform = rotate(transform, rotationSpeed, camera->rRight);
		camera->TransformCamera(transform);
		camera->transformMatrix = previousTransform;

	}

	if (a_Key == 224 || a_Key == 228)
	{
		mat4 previousTransform = transform;
		if (a_Key == 224)
			transform = rotate(transform, -1 * rotationSpeed, camera->viewDirection);
		else
			transform = rotate(transform, rotationSpeed, camera->viewDirection);
		camera->TransformCamera(transform);
		camera->transformMatrix = previousTransform;

	}
}

// -----------------------------------------------------------
// Main game tick function
// -----------------------------------------------------------
void Game::Tick(float dt)
{
	//renderSurface->Clear(0);
	HandleInput(dt);
	renderer->Render();

	char buffer[100];
	sprintf(buffer, "FPS: %f", 1 / dt);
	renderSurface->Print(buffer, 2, 2, 0xffffff);
}