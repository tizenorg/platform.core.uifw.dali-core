using UnityEngine;
using System.Collections;

public class DungeonBuilder : MonoBehaviour
{
    public Sprite empty;
    public Sprite wall;
    public Sprite room;
    public Sprite maze;
    public Sprite door;

    private StageBuilder stageBuilder;
    private Stage stage;

    public void Awake()
    {
        stageBuilder = new StageBuilder();
    }

    public void Start()
    {
        stage = stageBuilder.Build();

        Build();
    }

    private void Build()
    {
        for (int y = 0; y != stage.Height; ++y)
        {
            for (int x = 0; x != stage.Width; ++x)
            {
                Vector3 position = new Vector3(x, -y, 0.0f);
                Sprite sprite = GetSprite(stage[x, y]);

                CreateTile(position, sprite);
            }
        }
    }


    // TODO : Create the theme manager
    private Sprite GetSprite(Constant.Type type)
    {
        if (Constant.Type.Empty == type)
        {
            return empty;
        }
        else if (Constant.Type.Passage == type)
        {
            return maze;
        }
        else if (Constant.Type.Room == type)
        {
            return room;
        }
        else if (Constant.Type.Wall == type)
        {
            return wall;
        }
        else if (Constant.Type.Door == type)
        {
            return door;
        }
        return null;
    }

    private GameObject CreateTile(Vector3 position, Sprite sprite)
    {
        Object prefab = Resources.Load("Prefabs/Tile");
        GameObject tile = Instantiate(prefab, position, Quaternion.identity) as GameObject;
        tile.name = prefab.name;
        tile.transform.parent = gameObject.transform;
        tile.GetComponent<SpriteRenderer>().sprite = sprite;
        return tile;
    }
}
