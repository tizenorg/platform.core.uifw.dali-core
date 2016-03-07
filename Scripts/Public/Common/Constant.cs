using UnityEngine;
using System.Collections.Generic;

public class Constant
{
    /**
     * @brief Define the type of elements.
     */
    public enum Type : int
    {
        Empty,
        Wall,
        Room,
        Passage,
        Door
    }

    /**
     * @brief Define the direction can go.
     */
    public static readonly List<Vector2> Directions = new List<Vector2>()
    {
        new Vector2(1, 0),
        new Vector2(-1, 0),
        new Vector2(0, 1),
        new Vector2(0, -1)
    };
}
