using UnityEngine;

public interface IModifiable
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
        set;
    }

    Constant.Type this[Vector2 position]
    {
        set;
    }

    /**
     * @brief Check the position is inside the stage.
     *
     * @param[in] position The position.
     * @return True is the position is in the stage, false is not.
     */
    bool Inside(Vector2 position);
}