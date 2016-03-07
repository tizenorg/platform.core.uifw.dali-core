using UnityEngine;

public class Room : Element
{
    private Rect geometry;
    private IModifiable stage;

    public Rect Geometry
    {
        get
        {
            return geometry;
        }
    }

    /**
     * @brief Constructor.
     *
     * @param[in] geometry The room's geometry.
     * @param[in] stage The stage can be modified.
     */
    public Room(Rect geometry, IModifiable stage)
    {
        this.geometry = geometry;
        this.stage = stage;

        for (int y = (int)geometry.yMin; y != geometry.yMax; ++y)
        {
            for (int x = (int)geometry.xMin; x != geometry.xMax; ++x)
            {
                this.stage[x, y] = Constant.Type.Room;
            }
        }
    }

    /**
     * @copydoc Element::Inside()
     */
    public override bool Inside(Vector2 position)
    {
        return geometry.Contains(position);
    }
}
