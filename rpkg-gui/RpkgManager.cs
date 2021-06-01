using System.Collections.Generic;
using System.IO;
using System.Linq;
using rpkg.Extensions;

namespace rpkg
{
    static class RpkgManager
    {
        public delegate void RpkgEventHandler(string rpkgPath);

        public static event RpkgEventHandler RpkgLoaded;
        public static event RpkgEventHandler RpkgUnloaded;
        public static event RpkgEventHandler RpkgReloaded;

        private static readonly HashSet<string> _loadedRpkgs = new HashSet<string>();

        public static void LoadRpkg(string rpkgPath)
        {
            var fullPath = Path.GetFullPath(rpkgPath);
            LoadRpkgs(fullPath.ToSingleEnumerable());
        }

        public static void LoadRpkgsFromFolder(string rpkgFolder)
        {
            var fullFolderPath = Path.GetFullPath(rpkgFolder);
            var rpkgFiles = new HashSet<string>();

            foreach (var filePath in Directory.GetFiles(fullFolderPath, "*.rpkg"))
            {
                rpkgFiles.Add(filePath);
            }
            
            LoadRpkgs(rpkgFiles.SortNaturally());
        }

        public static void LoadRpkgs(IEnumerable<string> rpkgPaths)
        {
            // Here we group RPKGs based on their parent directory so we
            // can then load them in batches, based on that directory.
            var rpkgGroups = new Dictionary<string, HashSet<string>>();

            foreach (var rpkgPath in rpkgPaths)
            {
                var fullPath = Path.GetFullPath(rpkgPath);

                if (_loadedRpkgs.Contains(fullPath))
                    continue;

                var rpkgFolder = Path.GetDirectoryName(fullPath);

                if (!rpkgGroups.ContainsKey(rpkgFolder))
                    rpkgGroups.Add(rpkgFolder, new HashSet<string>());

                rpkgGroups[rpkgFolder].Add(fullPath);
            }

            foreach (var group in rpkgGroups)
            {
                LoadRpkgsInternal(group.Key, group.Value);
            }
        }

        public static void UnloadRpkg(string rpkgPath)
        {
            var fullPath = Path.GetFullPath(rpkgPath);

            if (!_loadedRpkgs.Contains(fullPath))
                return;

            RpkgLib.unload_rpkg(fullPath);
            _loadedRpkgs.Remove(fullPath);

            RpkgUnloaded?.Invoke(fullPath);
        }

        public static void ReloadRpkg(string rpkgPath)
        {
            var fullPath = Path.GetFullPath(rpkgPath);

            if (!_loadedRpkgs.Contains(fullPath))
                return;

            UnloadRpkg(rpkgPath);
            LoadRpkgsInternal(Path.GetDirectoryName(fullPath), fullPath.ToSingleEnumerable());

            // If we successfully reloaded it trigger the reloaded event.
            if (_loadedRpkgs.Contains(fullPath))
                RpkgReloaded?.Invoke(rpkgPath);
        }

        private static void LoadRpkgsInternal(string rpkgFolder, IEnumerable<string> rpkgPaths, bool quiet = false)
        {
            var rpkgPathsList = rpkgPaths.ToList();

            RpkgLib.reset_task_status();

            RpkgLib.execute_import_rpkgs temp_rpkgExecute = RpkgLib.import_rpkgs;

            temp_rpkgExecute.BeginInvoke(rpkgFolder, string.Join(",", rpkgPathsList) + ",", null, null);

            if (!quiet)
            {
                Progress progress = new Progress();

                progress.message.Content = $"Importing ${rpkgPathsList.Count} RPKG file(s)...";

                progress.operation = (int) Progress.Operation.MASS_EXTRACT;

                progress.ShowDialog();

                if (progress.task_status != (int) Progress.RPKGStatus.TASK_SUCCESSFUL)
                    return;
            }

            foreach (var rpkg in rpkgPathsList)
            {
                if (RpkgLib.is_rpkg_valid(rpkg) != 1)
                    continue;

                _loadedRpkgs.Add(rpkg);
                RpkgLoaded?.Invoke(rpkg);
            }
        }
    }
}
