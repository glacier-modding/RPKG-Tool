using rpkg.ResourceActions;
using rpkg.ResourceActions.Attributes;
using rpkg.Utils;

namespace rpkg.Resources.WWEM.Actions
{
    [ResourceAction("WWEM", "Convert to OGG (IOI Path)")]
    public class ConvertOggAction : IAction
    {
        public void Perform(string rpkgPath, string resourceHash)
        {
            var outputFolder = WindowUtils.SelectFolder("output", "Select Output Folder To Extract " + resourceHash + " To:");

            if (outputFolder == "")
                return;

            RpkgLib.reset_task_status();

            var progress = new Progress();

            progress.operation = (int)Progress.Operation.GENERAL;

            progress.message.Content = "Extracting " + resourceHash + " To OGG To IOI Path...";

            RpkgLib.task_execute("-extract_wwem_to_ogg_from", rpkgPath, resourceHash, "", "", outputFolder);

            progress.ShowDialog();
        }

        public void Dispose()
        {
        }
    }
}
