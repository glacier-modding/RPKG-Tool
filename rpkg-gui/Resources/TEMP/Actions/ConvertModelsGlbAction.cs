using rpkg.ResourceActions;
using rpkg.ResourceActions.Attributes;
using rpkg.Utils;

namespace rpkg.Resources.TEMP.Actions
{
    [ResourceAction("TEMP", "Convert linked PRIMs to GLB File(s)")]
    public class ConvertModelsGlbAction : IAction
    {
        public void Perform(string rpkgPath, string resourceHash)
        {
            var outputFolder = WindowUtils.SelectFolder("output", "Select Output Folder To Extract " + resourceHash + " To:");

            if (outputFolder == "")
                return;

            RpkgLib.reset_task_status();

            var progress = new Progress();

            progress.operation = (int)Progress.Operation.GENERAL;

            progress.message.Content = "Extracting PRIMs linked to " + resourceHash + " To GLB File(s)...";

            RpkgLib.task_execute("-extract_all_prim_of_temp_from", rpkgPath, resourceHash, "", "", outputFolder);

            progress.ShowDialog();
        }

        public void Dispose()
        {
        }
    }
}
