using System;
using System.IO;
using rpkg.ResourceActions;
using rpkg.ResourceActions.Attributes;
using rpkg.Utils;

namespace rpkg.Resources.TEMP.Actions
{
    [ResourceAction("TEMP", "Convert linked PRIM Models to GLB/TGA File(s)")]
    public class ConvertModelsGlbTgaAction : IAction
    {
        public void Perform(string rpkgPath, string resourceHash)
        {
            string runtimeDirectory = Path.GetDirectoryName(rpkgPath);

            if (!runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
            {
                WindowUtils.MessageBoxShow(
                    "The current RPKG does not exist in the Hitman runtime directory, can not perform PRIM model extraction."
                );

                return;
            }

            RpkgManager.LoadRpkgsFromFolder(runtimeDirectory);

            Progress progress = new Progress();

            progress.operation = (int)Progress.Operation.GENERAL;

            progress.message.Content = "Extracting PRIM Models linked to " + resourceHash + " To GLB/TGA File(s)...";

            var outputFolder = WindowUtils.SelectFolder("output", "Select Output Folder To Extract " + resourceHash + " To:");

            if (outputFolder == "")
                return;

            RpkgLib.reset_task_status();
            
            MainWindow.RebuildMODEL(
                "-extract_all_prim_model_of_temp_from",
                rpkgPath,
                resourceHash,
                "",
                "",
                outputFolder
            );

            progress.operation = (int)Progress.Operation.PRIM_MODEL_EXTRACT;

            progress.ShowDialog();

            if (progress.task_status != (int)Progress.RPKGStatus.PRIM_MODEL_EXTRACT_SUCCESSFUL)
            {
                WindowUtils.MessageBoxShow(progress.task_status_string.Replace("_", "__"));
            }
            else
            {
                WindowUtils.MessageBoxShow("PRIM model(s) extracted successfully in " + outputFolder);
            }
        }

        public void Dispose()
        {
        }
    }
}
