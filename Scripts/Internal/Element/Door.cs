using UnityEngine;

public class Door : Element
{
    private Vector2 geometry;
    private IModifiable stage;

    /**
     * @brief Constructor.
     *
     * @param[in] geometry The geometry of intersection.
     * @param[in] stage The stage can be modified.
     */
    public Door(Vector2 geometry, IModifiable stage)
    {
        this.geometry = geometry;
        this.stage = stage;

        this.stage[this.geometry] = Constant.Type.Door;
    }

    /**
     * @copydoc Element::Inside().
     */
    public override bool Inside(Vector2 position)
    {
        return geometry == position;
    }
}