using System;
using System.Collections.Generic;
using System.Linq;

public class Connector : IEquatable<Connector>
{
    private HashSet<Element> components;

    public int Count
    {
        get
        {
            return components.Count;
        }
    }

    /**
     * @brief Constructor.
     *
     * @param[in] components These are connected.
     */
    public Connector(HashSet<Element> components)
    {
        this.components = components;
    }

    /**
     * @brief Add the connected component.
     *
     * @param[in] component The connected component newly.
     */
    public void Add(Element component)
    {
        components.Add(component);
    }

    /**
     * @brief Determines whether the specified connector is eqauls to the current connector.
     *
     * @param[in] The connector to compare.
     * @return True is both are same, false is not.
     */
    public bool Equals(Connector other)
    {
        return Enumerable.SequenceEqual<Element>(components.OrderBy(component => component), other.components.OrderBy(component => component));
    }

    /**
     * @copydoc Object::GetHashCode()
     */
    public override int GetHashCode()
    {
        if (components.Count == 0)
        {
            return GetHashCode();
        }
        else
        {
            int hashCode = 0;
            foreach (Element component in components)
            {
                hashCode += component.GetHashCode();
            }
            return hashCode;
        }
    }
}
