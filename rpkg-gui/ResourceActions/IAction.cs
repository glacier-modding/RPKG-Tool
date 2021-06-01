using System;

namespace rpkg.ResourceActions
{
    interface IAction : IDisposable
    {
        void Perform(string rpkgPath, string resourceHash);
    }
}
