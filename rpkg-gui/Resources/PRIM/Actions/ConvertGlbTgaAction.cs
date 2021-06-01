using System;
using System.IO;
using rpkg.ResourceActions;
using rpkg.ResourceActions.Attributes;
using rpkg.Utils;

namespace rpkg.Resources.PRIM.Actions
{
    [ResourceAction("PRIM", "Convert MODEL to GLB/TGA File(s)")]
    public class ConvertGlbTgaAction : IAction
    {
        public void Perform(string rpkgPath, string resourceHash)
        {
            var runtimeDirectory = Path.GetDirectoryName(rpkgPath);

            if (!runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
            {
                WindowUtils.MessageBoxShow(
                    "The current RPKG does not exist in the Hitman runtime directory, can not perform PRIM model extraction."
                );

                return;
            }

            RpkgManager.LoadRpkgsFromFolder(runtimeDirectory);

            var outputFolder = WindowUtils.SelectFolder("output", "Select Output Folder To Extract " + resourceHash + " To:");

            if (outputFolder == "")
                return;

            RpkgLib.reset_task_status();

            var progress = new Progress();

            progress.operation = (int)Progress.Operation.GENERAL;

            progress.message.Content = "Extracting " + resourceHash + " To GLB/TGA File(s)...";

            MainWindow.ExtractMODEL("-extract_prim_model_from", rpkgPath, resourceHash, "", "", outputFolder);

            progress.ShowDialog();
        }

        public void Dispose()
        {
        }
    }
}
