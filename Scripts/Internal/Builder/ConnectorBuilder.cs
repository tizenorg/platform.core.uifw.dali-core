using UnityEngine;
using System.Collections.Generic;

public class ConnectorBuilder
{
    private Stage stage;
    private List<Room> rooms;
    private List<Passage> passages;
    private int linkedCount;

    public int LinkedCount
    {
        set
        {
            linkedCount = value;
        }

        get
        {
            return linkedCount;
        }
    }

    /**
     * @brief Constructor.
     *
     * @param[in] stage The stage.
     * @param[in] rooms The rooms.
     * @param[in] passages The passages.
     */
    public ConnectorBuilder(Stage stage, List<Room> rooms, List<Passage> passages)
    {
        this.stage = stage;
        this.rooms = rooms;
        this.passages = passages;

        linkedCount = 2;
    }

    /**
     * @brief Build the connectors with tunable values.
     *
     * @return The connectors. A connector has linked count to other at least.
     */
    public Dictionary<Connector, List<Vector2>> Build()
    {
        Dictionary<Connector, List<Vector2>> connectors = new Dictionary<Connector, List<Vector2>>();

        for (int y = 1; y != stage.Height - 1; ++y)
        {
            for (int x = 1; x != stage.Width - 1; ++x)
            {
                Vector2 pos = new Vector2(x, y);
                if (stage[pos] == Constant.Type.Empty)
                {
                    HashSet<Element> components = GetAdjacentComponents(pos);

                    if (components.Count >= linkedCount)
                    {
                        Connector connector = new Connector(components);

                        if (!connectors.ContainsKey(connector))
                        {
                            connectors.Add(connector, new List<Vector2>() { pos });
                        }
                        else
                        {
                            connectors[connector].Add(pos);
                        }
                    }
                }
            }
        }

        return connectors;
    }

    /**
     * @brief Retrieve the adjacent components at the position.
     *
     * @param[in] position The origin.
     * @return The components which are connected at the position.
     */
    private HashSet<Element> GetAdjacentComponents(Vector2 position)
    {
        HashSet<Element> components = new HashSet<Element>();
        foreach (Vector2 dir in Constant.Directions)
        {
            Vector2 adjacentPosition = position + dir;
            if (stage.Inside(adjacentPosition) && stage[adjacentPosition] != Constant.Type.Empty)
            {
                if (stage[adjacentPosition] == Constant.Type.Room)
                {
                    components.Add(FindComponent(rooms, adjacentPosition));
                }
                else if (stage[adjacentPosition] == Constant.Type.Passage)
                {
                    components.Add(FindComponent(passages, adjacentPosition));
                }
            }
        }

        return components;
    }

    /**
     * @brief Retrieve Component which has type T.
     *
     * @param[in] container It has components.
     * @param[in] position The key to find a container.
     */
    private Element FindComponent<T>(List<T> container, Vector2 position)
    {
        foreach (T element in container)
        {
            Element component = element as Element;
            if (component.Inside(position))
            {
                return component;
            }
        }
        return null;
    }
}