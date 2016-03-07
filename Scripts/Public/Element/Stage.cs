using UnityEngine;

public class Stage : IReadable, IModifiable
{
    private Constant.Type[,] map;

    public int Width
    {
        get
        {
            return map.GetLength(1);
        }
    }

    public int Height
    {
        get
        {
            return map.GetLength(0);
        }

    }

    public Constant.Type this[float x, float y]
    {
        get
        {
            return map[(int)y, (int)x];
        }

        set
        {
            map[(int)y, (int)x] = value;
        }
    }

    public Constant.Type this[Vector2 position]
    {
        get
        {
            return this[position.x, position.y];
        }

        set
        {
            this[position.x, position.y] = value;
        }
    }

    /**
     * @brief Constructor.
     *
     * @param[in] width The width of the stage.
     * @param[in] height The height of the stage.
     */
    public Stage(int width, int height)
    {
        map = new Constant.Type[height, width];

        for (int y = 0; y != Height; ++y)
        {
            for (int x = 0; x != Width; ++x)
            {
                this[x, y] = Constant.Type.Empty;
            }
        }
    }

    /**
     * @copydoc IModifiable::Inside()
     * @copydoc IReadable::Inside()
     */
    public bool Inside(Vector2 position)
    {
        Rect geometry = new Rect(0, 0, Width, Height);
        return geometry.Contains(position);
    }
}
