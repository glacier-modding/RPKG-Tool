using rpkg.ResourceActions;
using rpkg.ResourceActions.Attributes;
using rpkg.Utils;

namespace rpkg.Resources.PRIM.Actions
{
    [ResourceAction("PRIM", "Convert to GLB File")]
    public class ConvertGlbAction : IAction
    {
        public void Perform(string rpkgPath, string resourceHash)
        {
            var outputFolder = WindowUtils.SelectFolder("output", "Select Output Folder To Extract " + resourceHash + " To:");

            if (outputFolder == "")
                return;

            RpkgLib.reset_task_status();

            var progress = new Progress();

            progress.operation = (int)Progress.Operation.GENERAL;

            progress.message.Content = "Extracting " + resourceHash + " To GBL File...";

            RpkgLib.task_execute("-extract_prim_to_glb_from", rpkgPath, resourceHash, "", "", outputFolder);

            progress.ShowDialog();
        }

        public void Dispose()
        {
        }
    }
}
