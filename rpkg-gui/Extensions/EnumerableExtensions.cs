using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security;

namespace rpkg.Extensions
{
    static class EnumerableExtensions
    {
        [SuppressUnmanagedCodeSecurity]
        private static class SafeNativeMethods
        {
            [DllImport("shlwapi.dll", CharSet = CharSet.Unicode)]
            public static extern int StrCmpLogicalW(string psz1, string psz2);
        }
        
        public static List<string> SortNaturally(this IEnumerable<string> elements)
        {
            var list = elements.ToList();
            list.Sort(SafeNativeMethods.StrCmpLogicalW);
            return list;
        }

        public static IEnumerable<T> ToSingleEnumerable<T>(this T item)
        {
            yield return item;
        }
    }
}
