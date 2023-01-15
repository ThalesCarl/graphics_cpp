#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <string>

class PanAndZoom : public olc::PixelGameEngine
{
public:
    PanAndZoom()
    {
        sAppName = "PanAndZoom";
    }

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
        
        offsetX = 0.5 * ScreenWidth();
        offsetY = 0.5 * ScreenHeight();
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// called once per frame

        // Get Mouse Position (in the screen space)
        float mouseX = (float)GetMouseX();
        float mouseY = (float)GetMouseY();

        // std::cout << "MouseX = " << mouseX << ", MouseY = " << mouseY << std::endl;

        // capture the screen location when the user starts to pan
        if(GetMouse(0).bPressed)
        {
            startPanX = mouseX;
            startPanY = mouseY;
        }

        // as the mouse moves, the screen location changes. Convert this screen
        // coordinate change into world coordinates to implemnt the pan
        if(GetMouse(0).bHeld)
        {
            // std::cout << "before: " << "offSetX = " << offsetX << ", offsetY = " << offsetY << std::endl;
            offsetX += (mouseX - startPanX) / scaleX;
            offsetY += (mouseY - startPanY) / scaleY;
            //throw std::runtime_error("uepa");

            // Start "new" pan for next frame update
            startPanX = mouseX;
            startPanY = mouseY;
        }

        // Zooming
        float beforeZoom_mouseWorldX, beforeZoom_mouseWorldY;
        ScreenToWorld(mouseX, mouseY, beforeZoom_mouseWorldX, beforeZoom_mouseWorldY);

        if(GetMouseWheel())
        {
            int32_t mouseWheel = GetMouseWheel();
            if(mouseWheel > 0)
            {
                scaleX *= 1.01;
                scaleY *= 1.01;
            }
            else
            {
                scaleX *= 0.99;
                scaleY *= 0.99;
            }
        }
        
        // if(GetKey(olc::Q).bHeld)
        // {
        //     scaleX *= 1.001;
        //     scaleY *= 1.001;
        // }
        
        // if(GetKey(olc::A).bHeld)
        // {
        //     scaleX *= 0.999;
        //     scaleY *= 0.999;
        // }

        float afterZoom_mouseWorldX, afterZoom_mouseWorldY;
        ScreenToWorld(mouseX, mouseY, afterZoom_mouseWorldX, afterZoom_mouseWorldY);

        offsetX += (-beforeZoom_mouseWorldX + afterZoom_mouseWorldX);
        offsetY += (+beforeZoom_mouseWorldY - afterZoom_mouseWorldY);

        // Selected Cell
        if(GetMouse(1).bReleased)
        {
            selectedCellX = (int)afterZoom_mouseWorldX; // cast to floor value
            selectedCellY = (int)afterZoom_mouseWorldY;
        }
        // Clear the screen
        FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);

        // Clip the part of the world space that is not seen in the screen space
        float worldLeft, worldTop, worldBottom, worldRight;
        ScreenToWorld(0, 0, worldLeft, worldTop);
        ScreenToWorld(ScreenWidth(), ScreenHeight(), worldRight, worldBottom);

        // Display Grid of 10 horizontal lines
        for (float y = 0.0; y <= 10.0; y++)
        {
            if(y >= worldBottom && y <= worldTop)
            {
                float startX = 0.0, startY = y;
                float endX = 10.0, endY = y;

                int pixel_startX, pixel_startY, pixel_endX, pixel_endY; // Equivalents of above variables in the screen space

                WorldToScreen(startX, startY, pixel_startX, pixel_startY);
                WorldToScreen(endX, endY, pixel_endX, pixel_endY);
                
                DrawLine(pixel_startX, pixel_startY, pixel_endX, pixel_endY, olc::WHITE);
            }
        }

        // Display Grid of 10 vertical lines
        for (float x = 0.0; x <= 10.0; x++)
        {
            
            if(x >= worldLeft && x <= worldRight)
            {
                float startX = x, startY = 0.0;
                float endX = x, endY = 10.0;

                int pixel_startX, pixel_startY, pixel_endX, pixel_endY; // Equivalents of above variables in the screen space

                WorldToScreen(startX, startY, pixel_startX, pixel_startY);
                WorldToScreen(endX, endY, pixel_endX, pixel_endY);
                
                DrawLine(pixel_startX, pixel_startY, pixel_endX, pixel_endY, olc::WHITE);
            }
        }

        int cellX, cellY, cellRadius;
        WorldToScreen(selectedCellX + 0.5, selectedCellY + 0.5, cellX, cellY);
        cellRadius = 0.3 * scaleX;

        FillCircle(cellX, cellY, cellRadius, olc::RED);

        return true;
	}


private:
    // panning variables
    float offsetX = 0.0;
    float offsetY = 0.0;
    float startPanX = 0.0;
    float startPanY = 0.0;
    
    // zooming variables
    float scaleX = 1.0;
    float scaleY = 1.0;

    // Selecting cells
    float selectedCellX = 0.0;
    float selectedCellY = 0.0;

    void WorldToScreen(float worldX, float worldY, int& screenX, int& screenY)
    {
        screenX = (int)((offsetX + worldX) * scaleX);
        screenY = (int)((offsetY - worldY) * scaleY);
    }
    void ScreenToWorld(int screenX, int screenY, float& worldX, float& worldY)
    {
        worldX = (float)(screenX / scaleX - offsetX);
        worldY = (float)(offsetY - screenY / scaleY );
    }
};

int main()
{
	PanAndZoom demo;
	if (demo.Construct(600, 375, 6, 6))
		demo.Start();

	return 0;
}
