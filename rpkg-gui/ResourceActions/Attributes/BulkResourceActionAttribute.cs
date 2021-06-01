using System;

namespace rpkg.ResourceActions.Attributes
{
    [AttributeUsage(AttributeTargets.Class, Inherited = false)]
    class BulkResourceActionAttribute : Attribute
    {
        public string ResourceType { get; }

        public string Description { get; }

        public BulkResourceActionAttribute(string resourceType, string description)
        {
            ResourceType = resourceType;
            Description = description;
        }
    }
}
