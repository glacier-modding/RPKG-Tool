using System;

namespace rpkg.ResourceActions.Attributes
{
    [AttributeUsage(AttributeTargets.Class, Inherited = false)]
    class ResourceActionAttribute : Attribute
    {
        public string ResourceType { get; }

        public string Description { get; }

        public ResourceActionAttribute(string resourceType, string description)
        {
            ResourceType = resourceType;
            Description = description;
        }
    }
}
