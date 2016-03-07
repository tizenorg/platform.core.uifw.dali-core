using UnityEngine;

public interface IReadable
{
    int Width
    {
        get;
    }

    int Height
    {
        get;
    }

    Constant.Type this[float x, float y]
    {
        get;
    }

    Constant.Type this[Vector2 position]
    {
        get;
    }

    /**
     * @brief Check the position is inside the stage.
     *
     * @param[in] position The position.
     * @return True is the position is in the stage, false is not.
     */
    bool Inside(Vector2 position);
}