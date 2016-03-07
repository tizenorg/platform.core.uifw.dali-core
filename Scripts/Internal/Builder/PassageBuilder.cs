using UnityEngine;
using System.Collections.Generic;
using System.Linq;

public class PassageBuilder
{
    private Stage stage;
    private float probability;

    public float Probability
    {
        set
        {
            probability = value;
        }
        
        get
        {
            return probability;
        }
    }

    /**
     * @brief Constructor.
     *
     * @param[in] stage The stage.
     */
    public PassageBuilder(Stage stage)
    {
        this.stage = stage;

        Probability = 0.2f;
    }

    /**
     * @brief Build passages with tunable values.
     *
     * @return The passages.
     */
    public List<Passage> Build()
    {
        List<Passage> passages = new List<Passage>();

        for (int y = 1; y != stage.Height - 1; ++y)
        {
            for (int x = 1; x != stage.Width - 1; ++x)
            {
                Vector2 startPoint = new Vector2(x, y);

                if (stage[startPoint] == Constant.Type.Empty && ScanAdjacentPoints(startPoint, Constant.Directions))
                {
                    List<Vector2> geometry = CreateGeometry(startPoint);

                    // TODO: Let's think that the passage which has 1 length is important or not.
                    if (geometry.Count > 1)
                    {
                        Passage passage = new Passage(new LinkedList<Vector2>(geometry.ToArray()), stage);
                        passages.Add(passage);
                    }
                }
            }
        }

        return passages;
    }

    /**
     * @brief Scan adjacent points are clear(empty).
     *
     * @param[in] anchorPoint The anchor point to scan surroundings.
     * @param[in] direction The directions to scan.
     * @param[in] geometry It's additional information.
     * @return True is the surrounding of the anchor point is nothing.
     */
    private bool ScanAdjacentPoints(Vector2 anchorPoint, List<Vector2> directions, List<Vector2> geometry = null)
    {
        foreach (Vector2 dir in directions)
        {
            Vector2 adjacentPoint = anchorPoint + dir;

            if (!stage.Inside(adjacentPoint) || stage[adjacentPoint] != Constant.Type.Empty)
            {
                return false;
            }

            if (geometry != null)
            {
                if (geometry.Contains(adjacentPoint))
                {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * @brief Create the geometry of passage.
     *
     * @param[in] startPoint Passage start from this point.
     * @return The geometry of passage.
     */
    private List<Vector2> CreateGeometry(Vector2 startPoint)
    {
        List<Vector2> geometry = new List<Vector2>();
        geometry.Add(startPoint);

        Stack<Vector2> stack = new Stack<Vector2>();
        stack.Push(startPoint);

        Vector2 currentDirection = Vector2.zero;

        while (stack.Count != 0)
        {
            Vector2 currentPosition = stack.Peek();

            //////////////////////////////////////////////////////////////////////////////////////////////
            // Get the possible directions.
            //////////////////////////////////////////////////////////////////////////////////////////////
            Dictionary<Vector2, List<Vector2>> ways = new Dictionary<Vector2, List<Vector2>>();
            int distance = Random.Range(2, 4);
            foreach (Vector2 dir in Constant.Directions)
            {
                List<Vector2> way = new List<Vector2>();
                for (int dist = 0; dist != distance; ++dist)
                {
                    Vector2 possiblePoint = currentPosition + dir * (dist + 1);

                    if (stage.Inside(possiblePoint) && stage[possiblePoint] == Constant.Type.Empty && !geometry.Contains(possiblePoint))
                    {
                        if (ScanAdjacentPoints(possiblePoint, Constant.Directions.Where(d => d != -dir).ToList(), geometry))
                        {
                            way.Add(possiblePoint);
                        }
                    }
                }

                if (way.Count == distance)
                {
                    ways.Add(dir, way);
                }
            }

            //////////////////////////////////////////////////////////////////////////////////////////////
            // Select the direction with the randomness.
            //////////////////////////////////////////////////////////////////////////////////////////////
            if (ways.Count != 0)
            {
                if (currentDirection == Vector2.zero || !ways.ContainsKey(currentDirection) || probability >= Random.Range(0.0f, 1.0f))
                {
                    currentDirection = new List<Vector2>(ways.Keys)[Random.Range(0, ways.Count)];
                }

                List<Vector2> way = ways[currentDirection]; 
                geometry.AddRange(way);

                stack.Push(way[way.Count - 1]);
            }
            else
            {
                stack.Pop();
                currentDirection = Vector2.zero;
            }
        }

        return geometry;
    }
}