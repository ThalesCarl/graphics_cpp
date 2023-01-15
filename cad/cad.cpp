#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

// forward declatation
struct Shape;

struct Node
{
    Shape* parent;
    olc::vf2d position;
};

struct Shape
{
    std::vector<Node> nodes;
    uint32_t maxNumberOfNodes = 0;
    olc::Pixel color = olc::GREEN;

    static float worldScale;
    static olc::vf2d worldOffset;


    void WorldToScreen(const olc::vf2d& worldPosition, int& screenX, int& screenY)
    {
        screenX = (int)((worldOffset.x + worldPosition.x) * worldScale);
        screenY = (int)((worldOffset.y - worldPosition.y) * worldScale);
    }

    Node* GetNextNode(const olc::vf2d& position)
    {
        if(nodes.size() == maxNumberOfNodes)
            return nullptr; // The shape is complete, so no new nodes need to be created
        
        Node node;
        node.parent = this;
        node.position = position;
        nodes.push_back(node);
        return &nodes[nodes.size() -1];
    }

    // pure virtual function
    virtual void DrawYourself(olc::PixelGameEngine* pge) = 0;

    void DrawNodes(olc::PixelGameEngine* pge)
    {
        for(auto &node : nodes)
        {
            int sx, sy;
            WorldToScreen(node.position, sx, sy);
            pge->FillCircle(sx, sy, 2, olc::RED);
        }
    }

    Node* HitNode(olc::vf2d& position)
    {
        for (auto& node : nodes)
        {
            if((position - node.position).mag() < 0.01)
                return &node;
        }
        return nullptr;
    }
};

// initialization of static members of Shape struct
float Shape::worldScale = 1.0;
olc::vf2d Shape::worldOffset = {0, 0};

// Definition of shapes using the base class Shape
struct Line : public Shape
{
    Line()
    {
        maxNumberOfNodes = 2;
        nodes.reserve(maxNumberOfNodes); // Allocate a constant area of memory for our node vector
    }

    void DrawYourself(olc::PixelGameEngine* pge) override
    {
        int startX, startY, endX, endY;
        WorldToScreen(nodes[0].position, startX, startY);
        WorldToScreen(nodes[1].position, endX, endY);
        pge->DrawLine(startX, startY, endX, endY, color);
    }
};

struct Box : public Shape
{
    Box()
    {
        maxNumberOfNodes = 2;
        nodes.reserve(maxNumberOfNodes); // Allocate a constant area of memory for our node vector
    }

    void DrawYourself(olc::PixelGameEngine* pge) override
    {
        int startX, startY, endX, endY;
        WorldToScreen(nodes[0].position, startX, startY);
        WorldToScreen(nodes[1].position, endX, endY);
        pge->DrawRect(startX, startY, endX - startX, endY - startY, color);
    }
};

struct Circle : public Shape
{
    Circle()
    {
        maxNumberOfNodes = 2;
        nodes.reserve(maxNumberOfNodes); // Allocate a constant area of memory for our node vector
    }

    void DrawYourself(olc::PixelGameEngine* pge) override
    {
        float radius = (nodes[1].position - nodes[0].position).mag();
        int startX, startY, endX, endY;
        WorldToScreen(nodes[0].position, startX, startY);
        WorldToScreen(nodes[1].position, endX, endY);
        pge->DrawLine(startX, startY, endX, endY, color, 0xFF00FF00); // Dashed line representing the radius
        pge->DrawCircle(startX, startY, radius * worldScale, color);
    }
};

struct Spline : public Shape
{
    Spline()
    {
        maxNumberOfNodes = 3;
        nodes.reserve(maxNumberOfNodes); // Allocate a constant area of memory for our node vector
    }

    void DrawYourself(olc::PixelGameEngine* pge) override
    {
        int startX, startY, endX, endY;
        
        if(nodes.size() < 3)
        {
            // it can only draw line from first to second node
            WorldToScreen(nodes[0].position, startX, startY);
            WorldToScreen(nodes[1].position, endX, endY);
            pge->DrawLine(startX, startY, endX, endY, color, 0xFF00FF00); // Dashed line representing the radius
        }
        if(nodes.size() == 3)
        {
            // draw first structural line from first to second node
            WorldToScreen(nodes[0].position, startX, startY);
            WorldToScreen(nodes[1].position, endX, endY);
            pge->DrawLine(startX, startY, endX, endY, color, 0xFF00FF00); // Dashed line representing the radius

            // draw second structural line from second to third node
            WorldToScreen(nodes[1].position, startX, startY);
            WorldToScreen(nodes[2].position, endX, endY);
            pge->DrawLine(startX, startY, endX, endY, color, 0xFF00FF00); // Dashed line representing the radius

            // Draw the bezier curve as small lines
            olc::vf2d currentPoint = nodes[0].position;
            olc::vf2d nextPoint = currentPoint;
            for(float t = 0.0; t < 1.0; t += 0.01)
            {
                nextPoint = nodes[0].position * (1 - t)*(1 - t) + 
                               nodes[1].position * 2 * (1 - t)*t + 
                               nodes[2].position * t * t;
                WorldToScreen(currentPoint, startX, startY);
                WorldToScreen(nextPoint, endX, endY);
                pge->DrawLine(startX, startY, endX, endY, color);
                currentPoint = nextPoint;
            }

        }
    }
};
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

    // Shapes
    std::list<Shape*> shapesList;
    Shape* tempShape = nullptr;


    // Nodes
    Node* selectedNode = nullptr;

    olc::vf2d cursor = {0.0, 0.0}; // Defined because we want the drawing to snap to grid point

    void WorldToScreen(const olc::vf2d& worldPosition, int& screenX, int& screenY)
    {
        screenX = (int)((offset.x + worldPosition.x) * zoomScale);
        screenY = (int)((offset.y - worldPosition.y) * zoomScale);
    }
    void ScreenToWorld(int screenX, int screenY, olc::vf2d& worldPosition)
    {
        worldPosition.x = (float)(+ screenX / zoomScale - offset.x);
        worldPosition.y = (float)(- screenY / zoomScale + offset.y);
    }
public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
        offset = { (float)((0.5 * ScreenWidth()) * 1/zoomScale), (float)(0.5 * ScreenHeight() * 1/zoomScale)};
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
            offset += (mousePosition - startPan) / zoomScale;
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
        offset.x += (-mouseBeforeZoom.x + mouseAfterZoom.x);
        offset.y += (+mouseBeforeZoom.y - mouseAfterZoom.y);

        // Snap mouse cursor to nearest grid interval
        cursor.x = floorf((mouseAfterZoom.x + 0.5) * gridSize);
        cursor.y = floorf((mouseAfterZoom.y + 0.5) * gridSize);

        if(GetKey(olc::Key::L).bPressed)
        {
            tempShape = new Line();

            // Place first node at location of keypress
            selectedNode = tempShape->GetNextNode(cursor);

            // Start to get the second node
            selectedNode = tempShape->GetNextNode(cursor);
        }

        if(GetKey(olc::Key::B).bPressed)
        {
            tempShape = new Box();

            // Place first node at location of keypress
            selectedNode = tempShape->GetNextNode(cursor);

            // Start to get the second node
            selectedNode = tempShape->GetNextNode(cursor);
        }
        
        if(GetKey(olc::Key::C).bPressed)
        {
            tempShape = new Circle();

            // Place first node at location of keypress
            selectedNode = tempShape->GetNextNode(cursor);

            // Start to get the second node
            selectedNode = tempShape->GetNextNode(cursor);
        }

        if(GetKey(olc::Key::S).bPressed)
        {
            tempShape = new Spline();

            // Place first node at location of keypress
            selectedNode = tempShape->GetNextNode(cursor);

            // Start to get the second node
            selectedNode = tempShape->GetNextNode(cursor);
        }
        // Move the node under the cursor
        if(GetKey(olc::Key::M).bPressed)
        {
            selectedNode = nullptr;
            for(auto& shape : shapesList)
            {
                selectedNode = shape->HitNode(cursor);
                if(selectedNode != nullptr)
                    break;
            }
        }

        // Moving around with the next node of the shape
        if(selectedNode != nullptr)
        {
            selectedNode->position = cursor;
        }


        if(GetMouse(0).bReleased)
        {
            if(tempShape != nullptr)
            {
                selectedNode = tempShape->GetNextNode(cursor);
                if(selectedNode == nullptr) // The shape is completed
                {
                    tempShape->color = olc::WHITE;
                    shapesList.push_back(tempShape);
                }
            }
        }


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
        worldTopLeft.x = floor(worldTopLeft.x);
        worldTopLeft.y = floor(worldTopLeft.y);
        worldBottomRight.x = ceil(worldBottomRight.x);
        worldBottomRight.y = ceil(worldBottomRight.y);

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


        // Update shape static translation coefficientes
        Shape::worldScale = zoomScale;
        Shape::worldOffset = offset;

        // Draw the shapes from the list of completed shapes
        for(auto& shape : shapesList)
        {
            if(shape != nullptr)
            {
                shape->DrawYourself(this);
                shape->DrawNodes(this);
            }
        }

        // Draw the shape currently being constructed
        if(tempShape != nullptr)
        {
            tempShape->DrawYourself(this);
            tempShape->DrawNodes(this);
        }
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
