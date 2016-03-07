using UnityEngine;
using System.Collections.Generic;

public class Passage : Element
{
    private LinkedList<Vector2> geometry;
    private IModifiable stage;

    public LinkedListNode<Vector2> First
    {
        get
        {
            return geometry.First;
        }
    }

    public LinkedListNode<Vector2> Last
    {
        get
        {
            return geometry.Last;
        }
    }

    /**
     * @brief Constructor.
     *
     * @param[in] geometry The passage has.
     * @param[in] stage The stage can be modified.
     */
    public Passage(LinkedList<Vector2> geometry, IModifiable stage)
    {
        this.geometry = geometry;
        this.stage = stage;

        foreach (Vector2 position in geometry)
        {
            stage[position] = Constant.Type.Passage;
        }
    }

    /**
     * @brief Remove the position from the geometry.
     *
     * @param[in] position The position want to remove from a passage.
     * @return True is a position is removed, false is not.
     */
    public bool Remove(Vector2 position)
    {
        if (geometry.Remove(position))
        {
            stage[position] = Constant.Type.Empty;
            return true;
        }
        else
        {
            return false;
        }
    }

    /**
     * @brief Remove the position from the geometry.
     *
     * @param[in] position It is a node which has the position.
     */
    public void Remove(LinkedListNode<Vector2> position)
    {
        stage[position.Value] = Constant.Type.Empty;
        geometry.Remove(position);
    }

    /**
     * @copydoc Element::Inside()
     */
    public override bool Inside(Vector2 position)
    {
        return geometry.Contains(position);
    }
}