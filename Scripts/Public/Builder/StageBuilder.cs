using UnityEngine;
using System.Collections.Generic;

public class StageBuilder
{
    private int width;   // The width of the stage.
    private int height;  // The height of the stage.

    public int Width
    {
        set
        {
            width = value;
        }

        get
        {
            return width;
        }
    }

    public int Height
    {
        set
        {
            height = value;
        }

        get
        {
            return height;
        }
    }

    /**
     * @brief Default constructor.
     */
    public StageBuilder()
    {
        width = 89;
        height = 49;
    }

    /**
     * @brief Build the stage with tunable values.
     *
     * @return The builded stage which has rooms, maze and connectors.
     */
    public Stage Build()
    {
        // Create the stage.
        IReadable stage = new Stage(width, height);

        // Create the romms.
        List<Room> rooms = CreateRooms(stage as Stage);

        // Create the maze.
        List<Passage> passages = CreatePassages(stage as Stage);

        // Create the connectors and digging.
        CreateIntersections(stage as Stage, rooms, passages);

        // Remove the cul-de-sac passages.
        TrimPassages(stage as Stage, passages);

        // Create the walls.
        CreateWalls(stage as Stage);

        return stage as Stage;
    }

    /**
     * @brief Create the rooms with tunable values.
     *
     * @param[in] stage The stage.
     * @return The created rooms.
     */
    private List<Room> CreateRooms(Stage stage)
    {
        RoomBuilder roomBuilder = new RoomBuilder(stage);
        roomBuilder.Attemps = 100;
        roomBuilder.Gap = 1;
        return roomBuilder.Build();
    }

    /**
     * @brief Create the maze with tunable values.
     *
     * @param[in] stage The stage.
     * @return The created maze.
     */
    private List<Passage> CreatePassages(Stage stage)
    {
        PassageBuilder passageBuilder = new PassageBuilder(stage);
        passageBuilder.Probability = 0.25f;
        return passageBuilder.Build();
    }

    /**
     * @brief Create the connectors with tunable values.
     *
     * @param[in] stage The stage.
     * @param[in] rooms The rooms.
     * @param[in] passages The passages.
     * @return The created connectors
     */
    private List<Door> CreateIntersections(Stage stage, List<Room> rooms, List<Passage> passages)
    {
        DoorBuilder intersectionBuilder = new DoorBuilder(stage, rooms, passages);

        return intersectionBuilder.Build();
    }

    /**
     * @brief Remove the cul-de-sac passage from the stage.
     *
     * @param[in] stage The stage.
     * @param[in] passages It has the rough passages.
     */
    private void TrimPassages(Stage stage, List<Passage> passages)
    {
        // Trim passages from the last.
        foreach (Passage passage in passages)
        {
            LinkedListNode<Vector2> current = passage.Last;
            while (current != null)
            {
                LinkedListNode<Vector2> prev = current.Previous;

                if (CulDeSac(stage, current.Value))
                {
                    passage.Remove(current);
                }
                current = prev;
            }
        }

        // Trim passages from the first.
        foreach (Passage passage in passages)
        {
            LinkedListNode<Vector2> current = passage.First;
            while (current != null)
            {
                LinkedListNode<Vector2> next = current.Next;

                if (CulDeSac(stage, current.Value))
                {
                    passage.Remove(current);
                }
                current = next;
            }
        }
    }

    /**
     * @brief Check the current position is cul-de-sack or not.
     *
     * @param[in] stage The stage.
     * @param[in] position The position on the stage.
     * @return True if the position is cul-de-sack, false the position is connected with something.
     */
    private bool CulDeSac(Stage stage, Vector2 position)
    {
        int count = 0;
        foreach (Vector2 dir in Constant.Directions)
        {
            Vector2 pos = position + dir;
            if (stage[pos] == Constant.Type.Empty)
            {
                if (++count >= 3)
                {
                    return true;
                }
            }
        }
        return false;
    }

    private List<Wall> CreateWalls(Stage stage)
    {
        WallBuilder wallBuilder = new WallBuilder(stage);
        return wallBuilder.Build();
    }
}
