#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class CadProgram : public olc::PixelGameEngine
{
public:
	CadProgram()
	{
		sAppName = "CadProgram";
	}

private:
    olc::vf2d offset = {0.0, 0.0};
    olc::vf2d startPan = {0.0, 0.0};
    float zoomScale = 10.0;
    float gridSize = 1.0; // defined in world space

    olc::vf2d cursor = {0.0, 0.0}; // Defined because we want the drawing to snap to grid point

    void WorldToScreen(const olc::vf2d& worldPosition, int& screenX, int& screenY)
    {
        screenX = (int)((worldPosition.x - offset.x) * zoomScale);
        screenY = (int)((worldPosition.y - offset.y) * zoomScale);
    }
    void ScreenToWorld(int screenX, int screenY, olc::vf2d& worldPosition)
    {
        worldPosition.x = (float)(screenX / zoomScale + offset.x);
        worldPosition.y = (float)(screenY / zoomScale + offset.y);
    }
public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
        offset = { (float)((-0.5 * ScreenWidth()) * 1/zoomScale), (float)(-0.5 * ScreenHeight() * 1/zoomScale)};
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// called once per frame
        olc::vf2d mousePosition = { (float) GetMouseX(), (float) GetMouseY()};

        // Panning
        if(GetMouse(2).bPressed) // Check if the wheel is pressed
        {
            startPan = mousePosition;
        }
        if(GetMouse(2).bHeld) // The user is panning the view
        {
            offset -= (mousePosition - startPan) / zoomScale;
            startPan = mousePosition;
        }

        // Zooming
        olc::vf2d mouseBeforeZoom;
        ScreenToWorld((int)mousePosition.x, (int)mousePosition.y, mouseBeforeZoom);
        if (GetKey(olc::Key::Q).bHeld || GetMouseWheel() > 0) // The user can zoom in helding the Q key or spinning the mouse wheel forwards
        {
            zoomScale *= 1.1;
        }
        if (GetKey(olc::Key::A).bHeld || GetMouseWheel() < 0) // The user can zoom out helding the A key or spinning the mouse wheel backwards
        {
            zoomScale *= 0.9;
        }
        olc::vf2d mouseAfterZoom;
        ScreenToWorld((int)mousePosition.x, (int)mousePosition.y, mouseAfterZoom);
        offset += (mouseBeforeZoom - mouseAfterZoom);

        // Snap mouse cursor to nearest grid interval
        cursor.x = floorf((mouseAfterZoom.x + 0.5) * gridSize);
        cursor.y = floorf((mouseAfterZoom.y + 0.5) * gridSize);

        // Clear Screen
        Clear(olc::VERY_DARK_BLUE);

        // Auxiliar variables used to hold positions on screen space
        int sx, sy;
        int ex, ey;

        // Get visible world
        olc::vf2d worldTopLeft, worldBottomRight;
        ScreenToWorld(0, 0, worldTopLeft);
        ScreenToWorld(ScreenWidth(), ScreenHeight(), worldBottomRight);

        // Round up the values of the edges in order to avoid lines to be clipped
        //worldTopLeft.x = floor(worldTopLeft.x);
        //worldTopLeft.y = floor(worldTopLeft.y);
        //worldBottomRight.x = ceil(worldBottomRight.x);
        //worldBottomRight.y = ceil(worldBottomRight.y);

        // Draw grid dots
        for (float x = worldTopLeft.x; x < worldBottomRight.x; x += 10.0 * gridSize)
        {
            for (float y = worldTopLeft.y; y < worldBottomRight.y; y += 10.0 * gridSize)
            {
                WorldToScreen({x, y}, sx, sy);
                Draw(sx, sy, olc::BLUE);
            }
        }


        // Draw world axis (dotted line)
        WorldToScreen({0, worldTopLeft.y}, sx, sy);
        WorldToScreen({0, worldBottomRight.y}, ex, ey);
        DrawLine(sx, sy, ex, ey, olc::GREY, 0xF0F0F0F0); // The pattern is four bits on, four bits off and repeat
        WorldToScreen({worldTopLeft.x, 0}, sx, sy);
        WorldToScreen({worldBottomRight.x, 0}, ex, ey);
        DrawLine(sx, sy, ex, ey, olc::GREY, 0xF0F0F0F0); // The pattern is four bits on, four bits off and repeat
        //DrawString({0, 0}, std::string("mouse = " + mouseAfterZoom.str()));
        //DrawString({0, 40}, std::string("offset = " + offset.str()));

        // Draw cursor
        WorldToScreen(cursor, sx, sy);
        DrawCircle(sx, sy, 3, olc::YELLOW);
        DrawString({10, 10}, cursor.str(), olc::YELLOW, 2);


		return true;
	}
};


int main()
{
	CadProgram demo;
	if (demo.Construct(1000, 600, 1, 1))
		demo.Start();

	return 0;
}
