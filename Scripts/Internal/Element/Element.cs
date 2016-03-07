using UnityEngine;
using System;

public abstract class Element : IComparable<Element>
{
    /**
     * @copydoc IComparable::CompareTo()
     */
    public int CompareTo(Element component)
    {
        return GetHashCode() - component.GetHashCode();
    }

    /**
     * @brief Check the position is in the geometry.
     *
     * @param[in] position The position.
     * @return True is the position is in the geometry, false is not.
     */
    public abstract bool Inside(Vector2 position);
}