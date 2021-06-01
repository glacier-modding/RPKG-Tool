using System;

namespace rpkg.ResourceActions
{
    interface IBulkAction : IDisposable
    {
        void PerformBulk(string rpkgPath, string resourceType);
    }
}
