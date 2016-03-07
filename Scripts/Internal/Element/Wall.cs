using System;
using UnityEngine;

public class Wall : Element
{
    private Vector2 geometry;
    private IModifiable stage;

    /**
     * @brief Constructor.
     *
     * @param[in] geometry The geometry of wall.
     * @param[in] stage The stage can be modified.
     */
    public Wall(Vector2 geometry, IModifiable stage)
    {
        this.geometry = geometry;
        this.stage = stage;

        this.stage[this.geometry] = Constant.Type.Wall;
    }

    /**
     * @copydoc Element::Inside()
     */
    public override bool Inside(Vector2 position)
    {
        return geometry == position;
    }
}
