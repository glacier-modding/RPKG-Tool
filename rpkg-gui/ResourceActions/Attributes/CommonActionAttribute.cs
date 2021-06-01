using System;

namespace rpkg.ResourceActions.Attributes
{
    [AttributeUsage(AttributeTargets.Class, Inherited = false)]
    class CommonActionAttribute : Attribute
    {
        public string Description { get; }

        public CommonActionAttribute(string description)
        {
            Description = description;
        }
    }
}
