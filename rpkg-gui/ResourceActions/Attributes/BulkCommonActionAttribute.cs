using System;

namespace rpkg.ResourceActions.Attributes
{
    [AttributeUsage(AttributeTargets.Class, Inherited = false)]
    class BulkCommonActionAttribute : Attribute
    {
        public string Description { get; }

        public BulkCommonActionAttribute(string description)
        {
            Description = description;
        }
    }
}
