using UnityEngine;
using System.Collections.Generic;

public class RoomBuilder {

    private Stage stage;
    private int attemps;
    private int widthMin;
    private int widthMax;
    private int heightMin;
    private int heightMax;

    private int gap;

    public int Attemps
    {
        set
        {
            attemps = value;
        }
        
        get
        {
            return attemps;
        }
    }

    public int WidthMin
    {
        set
        {
            widthMin = value;
        }

        get
        {
            return widthMin;
        }
    }

    public int WidthMax
    {
        set
        {
            widthMax = value;
        }

        get
        {
            return widthMax;
        }
    }

    public int HeightMin
    {
        set
        {
            heightMin = value;
        }

        get
        {
            return heightMin;
        }
    }

    public int HeightMax
    {
        set
        {
            heightMax = value;
        }

        get
        {
            return heightMax;
        }
    }

    public int Gap
    {
        set
        {
            gap = value;
        }

        get
        {
            return gap;
        }
    }

    /**
     * @brief Constructor.
     *
     * @param[in] stage The stage.
     */
    public RoomBuilder(Stage stage)
    {
        this.stage = stage;
        widthMin = 10;
        widthMax = 30;
        heightMin = 10;
        heightMax = 20;
        gap = 1;
    }

    /**
     * @brief Build rooms with tunable values.
     *
     * @return Rooms.
     */
    public List<Room> Build()
    {
        List<Room> rooms = new List<Room>();

        for (int count = 0; count != attemps; ++count)
        {
            Rect geometry = CreateRandomGeometry();

            if (!Overlaps(geometry, rooms))
            {
                Room room = new Room(geometry, stage);
                rooms.Add(room);
            }
        }

        return rooms;
    }

    /**
     * @brief Generate the geometry of room randomly.
     *
     * @return The geometry of room.
     */
    private Rect CreateRandomGeometry()
    {
        int width = Random.Range(widthMin, widthMax);
        int height = Random.Range(heightMin, heightMax);
        int x = Random.Range(1, stage.Width - 2 - width);
        int y = Random.Range(1, stage.Height - 2 - height);

        return new Rect(x, y, width, height);
    }

    /**
     * @brief Check the geometry is overlaped with other room's geometry.
     *
     * @param[in] geometry The Geometry of the room.
     * @param[in] rooms The rooms.
     * @return True is the geometry is overlaped with other, false is not.
     */
    private bool Overlaps(Rect geometry, List<Room> rooms)
    {
        foreach (Room other in rooms)
        {
            Rect geometryWithGap = new Rect(geometry.x - gap, geometry.y - gap, geometry.width + gap * 2, geometry.height + gap * 2);
            if (geometryWithGap.Overlaps(other.Geometry))
            {
                return true;
            }
        }
        return false;
    }
}
