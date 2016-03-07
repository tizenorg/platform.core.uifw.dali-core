using UnityEngine;
using System.Collections.Generic;

class WallBuilder
{
    Stage stage;

    /**
     * @brief Constructor.
     *
     * @param[in] stage The stage can be modified and readable.
     */
    public WallBuilder(Stage stage)
    {
        this.stage = stage;
    }

    /**
     * @brief Build walls.
     *
     * @return The walls.
     */
    public List<Wall> Build()
    {
        List<Wall> walls = new List<Wall>();

        for (int y = 0; y != stage.Height; ++y)
        {
            for (int x = 0; x != stage.Width; ++x)
            {
                Vector2 pos = new Vector2(x, y);
                if (stage[pos] == Constant.Type.Empty && RightPlaceBuildingWall(pos))
                {
                    Wall wall = new Wall(pos, stage);
                    walls.Add(wall);
                }
            }
        }
        return walls;
    }

    /**
     * @brief Check the right place to build a wall.
     *
     * @param[in] position The position.
     * @return True is the correct position to make wall, false is not.
     */
    private bool RightPlaceBuildingWall(Vector2 position)
    {
        for (int y = -1; y != 2; ++y)
        {
            for (int x = -1; x != 2; ++x)
            {
                Vector2 dir = new Vector2(x, y);
                Vector2 pos = position + dir;
                if (stage.Inside(pos))
                {
                    if (stage[pos] != Constant.Type.Empty && stage[pos] != Constant.Type.Wall)
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }
}