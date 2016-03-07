using UnityEngine;
using System.Collections.Generic;

public class DoorBuilder
{
    Stage stage;
    List<Room> rooms;
    List<Passage> passages;

    /**
     * @brief Constructor.
     *
     * @param[in] stage The stage.
     * @param[in] rooms The rooms.
     * @param[in] passage The passages.
     */
    public DoorBuilder(Stage stage, List<Room> rooms, List<Passage> passages)
    {
        this.stage = stage;
        this.rooms = rooms;
        this.passages = passages;
    }

    /**
     * @brief Build the intersections.
     *
     * @return The intersections.
     */
    public List<Door> Build()
    {
        ConnectorBuilder connectorBuilder = new ConnectorBuilder(stage, rooms, passages);
        Dictionary<Connector, List<Vector2>> connectors = connectorBuilder.Build();


        List<List<Vector2>> groups = new List<List<Vector2>>();
        foreach (KeyValuePair<Connector, List<Vector2>> keyValuePair in connectors)
        {
            foreach (Vector2 position in keyValuePair.Value)
            {
                List<Vector2> group = FindGroup(position, groups);

                if (group != null)
                {
                    group.Add(position);
                }
                else
                {
                    groups.Add(new List<Vector2>() { position });
                }
            }
        }

        List<Door> intersections = new List<Door>();
        foreach (List<Vector2> group in groups)
        {
            // TODO : How many intersection are created in one group?
            Vector2 pos = group[Random.Range(0, group.Count)];
            intersections.Add(new Door(pos, stage));
        }

        return null;
    }

    /**
     * @brief Retrieve the group.
     *
     * @param[in] position The key to find a group.
     * @param[in] groups It has unclassified positions.
     * @return The finded group, if it is null then the group is not exist.
     */
    private List<Vector2> FindGroup(Vector2 position, List<List<Vector2>> groups)
    {
        foreach (List<Vector2> group in groups)
        {
            foreach (Vector2 pos in group)
            {
                foreach (Vector2 dir in Constant.Directions)
                {
                    if (position == pos + dir)
                    {
                        return group;
                    }
                }
            }
        }
        return null;
    }
}
